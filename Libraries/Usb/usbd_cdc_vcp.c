#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "osConfig.h"

#include  <string.h>

//extern uint8_t replayWHh41[SIZE_BUFF_WH41];
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
	uint32_t command = *(uint32_t*)buffer;
	char *mess;
	switch(command)
	{
		case 02:
			mess = "Get command ReadName";
			xQueueSendFromISR(cansolQueue, &mess, 0);

			DCD_EP_Tx(pdev, 02, "Hello", 6);
			break;
		case COM_USB_SEND_MESSAGE:
			usbSenMessToWT41(&buffer[4], Len - 4);
			break;
		case COM_USB_GET_MESSAGE:
			usbReplayGetMessage(pdev);
			break;
	}

	return USBD_OK;
}

const char *usb_txt[] = { "DCD_HandleOutEP_ISR", //0
		"DCD_HandleInEP_ISR", //1
		"USB_OTG_WRITE_REG32" };

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
//	static char *cRxChar = "hello ";
//	replayWHh41[0] = 'a';
//	replayWHh41[1] = 'b';
//	replayWHh41[2] = 0;
	if( fUart2Usb == 1 )
	{
		DCD_EP_Tx(pdev, 02, (uint8_t*)&rfd_buffer, itWh41);
		itWh41 = 0;
		fUart2Usb = 0;
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
