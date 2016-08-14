#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "osConfig.h"
#include "structCommon.h"
#include "../../tasks/usbTask.h"
#include "../../tasks/configTerem.h"

#include  <string.h>

extern uint8_t fUart2Usb;
extern uint8_t rfd_buffer;
extern int itWh41;

USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN 
 transfer management. */
extern uint8_t APP_Rx_Buffer[]; /* Write CDC received data in this buffer.
 These data will be sent over USB IN endpoint
 in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in; /* Increment this pointer or roll it back to
 start address when writing received data
 in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_DataTx(void);
static uint16_t VCP_DataRx(uint8_t* Buf, uint32_t Len, void *pdev);

CDC_IF_Prop_TypeDef VCP_fops = { VCP_DataTx, VCP_DataRx };
static uint16_t VCP_DataTx(void)
{
	if(fUart2Usb == 1)
		fUart2Usb = 0;
	return USBD_OK;
}

uint32_t sizeNextPaket = 0;

static uint16_t VCP_DataRx(uint8_t* buffer, uint32_t Len, void *pdev)
{
	static int itUsbBuf = 0;
	memcpy((void*)&usbBuffer[itUsbBuf], buffer, Len);
	if(Len == 64 )
	{
		itUsbBuf += 64;
		return USBD_OK;
	}
	else
		itUsbBuf += Len;

	uint32_t command = *(uint32_t*)usbBuffer;
	command &= 0xff;
	char *mess;
	switch(command)
	{
		case 02:
			mess = "Get command ReadName";
			xQueueSendFromISR(cansolQueue, &mess, 0);

			DCD_EP_Tx(pdev, 02, "Hello", 6);
			break;
		case COM_USB_SEND_MESSAGE:
			usbSenMessToWT41(&usbBuffer[4], itUsbBuf - 4);
			break;
		case COM_USB_GET_MESSAGE:
			usbReplayGetMessage(pdev);
			break;
		case COM_USB_READ_MEM:
			usbReadMemory(usbBuffer, pdev);
			break;
		case COM_USB_READ_ADDRESS:
			usbReadAddress(buffer, pdev);
			break;
		case COM_USB_WRITE_ADDRESS:
			usbWriteAddress(usbBuffer, pdev);
			break;
		case COM_USB_RF_COM:
			usbRfComand(usbBuffer, pdev, itUsbBuf);
			break;
	}
	itUsbBuf = 0;

	return USBD_OK;
}

void usbRfComand(uint8_t* buffer, void *pdev, uint32_t Len)
{
	uint32_t command = *(uint32_t*)buffer;
	uint8_t numFrame = (command >> 16) &0xff;//����� �����
	uint8_t countFrame = (command >> 24) &0xff;//����p� ���-�� ������

	//memcpy((void*)&usbBuffer[numFrame * 60], buffer+4, Len - 4);

	if(numFrame == countFrame)
	{//���� ����� �������
		xEventGroupSetBitsFromISR(xEventGroup, FLAG_COM_USB, 0);
	}
}

void usbReadAddress(uint8_t* buffer, void *pdev)
{
	buffer[0] = teremParam.address;
	DCD_EP_Tx(pdev, 02, (uint8_t*)&teremParam.address, 1);
}

/* ������� usbWriteAddress ��������� �����.
 * ������ 4 ����� � ������ - ��� ������� (7)
 * buffer[5] - �����
 * � ����� ����� ���������� �����
 */
void usbWriteAddress(uint8_t* buffer, void *pdev)
{
	teremParam.address = buffer[4];
	xEventGroupSetBitsFromISR(xEventGroup, FLAG_WRITE_PARAM, 0);
	DCD_EP_Tx(pdev, 02, (uint8_t*)&teremParam.address, 1);
}


/* ������� usbReadMemory ������ ������.
 * ������ 4 ����� � ������ - ��� ������� (5)
 * buffer[4] - ��� ������� 0 - ���
 * 1 - Flas, 2 - SPI Flash, 3 - I2C EEPROM
 * buffer[5] - ������ � ������ ������������� �����.
 * �� ������� ��� �� 1 ����� �� ����� �������� ������ 64 ����,
 * �� ������ ������ ���� ������ 64
 * buffer[6]-buffer[7] �����
 * buffer[8]-buffer[12] - ������ � ������ �� ���� ������
 *
 */
void usbReadMemory(uint8_t* buffer, void *pdev)
{
	//static uint8_t data[16];
	uint32_t address = u32FromU8(&buffer[8]);
	uint16_t size = buffer[5];
	if(size > 64)
		size = 64;
	switch(buffer[4])
	{
		case 0: //RAM
			DCD_EP_Tx(pdev, 02, (uint8_t*)address, size);
			break;
		case 1: //Flash
			break;
		case 2: //MX25L64 (SPI)
		{//��������, ���� ����� ��� ����������
			if(address < 8388608)
			{
				MemCom com;
				com.address = address;
				com.count = size;
				xQueueSendFromISR(memComUsbQueue, &com, 0);
				//DCD_EP_Tx(pdev, 02, data, size);
			}
		}
			break;
		case 3: //AT24C256 (I2C)
			break;
		default:
			return;
	}
}

//const char *usb_txt[] = { "DCD_HandleOutEP_ISR", //0
//		"DCD_HandleInEP_ISR", //1
//		"USB_OTG_WRITE_REG32" };

static char mess[100];
void usbSenMessToWT41(uint8_t *buf, uint32_t Len)
{
	if(Len > 99)
		Len = 99;
	memcpy((void*)mess, (void*)buf, Len);
	mess[Len] = 0;
	char *p = mess;
	xQueueSendFromISR(cansolQueue, &p, 0);
}

void usbReplayGetMessage(void *pdev)
{
<<<<<<< HEAD
	static int count1 = 0;//������� ���������
	static int count2 = 0;
	count2 = itWh41;

	if(count2 != count1)
	{
		int size = count2 - count1;
		if(size < 0)
			size = SIZE_BUFF_WH41 - count1;
		if(size > 64)
			size = 64;
		DCD_EP_Tx(pdev, 02, ((uint8_t*)&replayWHh41) + count1, size);
		count1 += size;
		if(count1 >= SIZE_BUFF_WH41)
			count1 -= SIZE_BUFF_WH41;
=======
	if( fUart2Usb == 1 )
	{
		DCD_EP_Tx(pdev, 02, (uint8_t*)&rfd_buffer, itWh41);
		itWh41 = 0;
		fUart2Usb = 0;
>>>>>>> 86d32411d2a039939808d87e72e74849c2a07816
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
