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


#include <string.h>

extern uint8_t rfd_buffer[4120];
extern int endTransmit;
extern uint16_t rfd_count;	//счетчик принятых/отправленных байт
extern uint8_t rfd_sizeOfFrame; //длинна пакета;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;


void usbTask(void *context)
{

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS
            USB_OTG_HS_CORE_ID,
#else
            USB_OTG_FS_CORE_ID,
#endif
            &USR_desc,
            &USBD_CDC_cb,
            &USR_cb);

  NVIC_SetPriority(OTG_FS_IRQn, 13);
  static uint32_t aaa = NVIC_GetPriority(OTG_FS_IRQn);

  strcpy((char*)rfd_buffer, "Hello");
  rfd_sizeOfFrame = 5;
  rfd_count = 0;
  			USART_ClearITPendingBit(USART2, USART_IT_TC);
  					USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
  					USART_SendData(USART2, rfd_buffer[0]);



  	 char *message;
	while(1)
	{
          
                                        
		if(xQueueReceive(cansolQueue,  &message , ( TickType_t ) 10 ) )
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
                                        while(endTransmit != 0) vTaskDelay(1);

		} 
                /*
//		GPIO_SetBits(GPIOA, GPIO_Pin_10);
		GPIO_SetBits(GPIOC, GPIO_Pin_2);
		vTaskDelay(50);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		vTaskDelay(50);
		asm("nop");*/
		//USART_SendData(USART2, 0x55);
	}
}



