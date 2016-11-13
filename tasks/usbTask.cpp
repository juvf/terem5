/*
 * usbTask.cpp
 *
 *  Created on: 29 апр. 2015 г.
 *      Author: juvf
 */
#include "usbTask.h"
#include "osConfig.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "../structCommon.h"
#include "../flashMx25.h"
#include "Checksum.h"
#include "taskUartRfd.h"

#include <string.h>

extern uint8_t rfd_buffer[];
extern int endTransmit;
extern uint16_t rfd_count;	//счетчик принятых/отправленных байт
extern uint8_t rfd_sizeOfFrame; //длинна пакета;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

uint8_t usbBuffer[280];

void usbTask(void *context)
{

	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET )
		xEventGroupClearBits(xEventGroup, FLAG_USB_NO_POWER);
	else
		xEventGroupSetBits(xEventGroup, FLAG_USB_NO_POWER);

	char *message;
	MemCom com;
	while(1)
	{

		if( !((xEventGroupGetBits(xEventGroup) & FLAG_USB_NO_POWER) == FLAG_USB_NO_POWER) )
		{ //есть питание USB. Проверим - нужен ли инит?
			if( (xEventGroupGetBits(xEventGroup) & FLAG_USB_INIT) == 0 )
			{
				initialUsb();
				xEventGroupSetBits(xEventGroup, FLAG_USB_INIT);
			}
		}
		else
		{ //питание пропало.
			if( (xEventGroupGetBits(xEventGroup) & FLAG_USB_INIT) == FLAG_USB_INIT )
			{ //Деинициализируем УСБ
				deinitialUsb();
				xEventGroupClearBits(xEventGroup, FLAG_USB_INIT);
			}
			xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_USB);
			vTaskDelay(10);
			continue;
		}
#ifdef USB_CONSOL
		if( xQueueReceive(cansolQueue, &message, (TickType_t ) 10) )
		{
			//выдадим сообщение
			strcpy((char*)rfd_buffer, message);
			strcat((char*)rfd_buffer, "\r\n");
			endTransmit = 1;
			rfd_sizeOfFrame = strlen((char*)rfd_buffer);
			rfd_count = 0;
			USART_ClearITPendingBit(USART2, USART_IT_TC);
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			USART_SendData(USART2, rfd_buffer[0]);
			while(endTransmit != 0)
				vTaskDelay(1);

		}
#endif
		if( xQueueReceive(memComUsbQueue, &com, 5) )
		{
			flashMx25Read(usbBuffer, com.address, com.count);
			DCD_EP_Tx(&USB_OTG_dev, 02, usbBuffer, com.count);
			vTaskDelay(1);
		}

		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, FLAG_COM_USB,
		pdTRUE, pdFALSE, 10);
		if( (uxBits & FLAG_COM_USB) == FLAG_COM_USB )
		{ //пришла команда радиоканальная по УСБ. проверим ЦРЦ
			uint8_t *pBuf = usbBuffer + 4;
			uint8_t length = pBuf[1];
			if( Checksum::crc16(pBuf, length) == 0 )
			{
				parser(pBuf, false);
				if( pBuf[1] > 0 )
				{
					bool isNeedNuulFrame = (pBuf[1] % 64) == 0;
					if( isNeedNuulFrame )
						pBuf[1]++;
					DCD_EP_Tx(&USB_OTG_dev, 02, pBuf, pBuf[1]);
				}
				vTaskDelay(100);
			}
		}
	}
}

void initialUsb()
{
	USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
			USB_OTG_HS_CORE_ID,
#else
			USB_OTG_FS_CORE_ID,
#endif
			&USR_desc, &USBD_CDC_cb, &USR_cb);

	NVIC_SetPriority(OTG_FS_IRQn, 13);
}

void deinitialUsb()
{
	//отключим клоки от УСБ.
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USE_USB_OTG_FS

//	/* Configure SOF ID DM DP Pins */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USB_OTG_CORE_HANDLE *pdev = &USB_OTG_dev;
	USB_OTG_WRITE_REG32(&pdev->regs.GREGS->GCCFG, 0x0ffff);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, DISABLE);

	NVIC_DisableIRQ(OTG_FS_IRQn);

#endif /* USB_OTG_FS */
}
