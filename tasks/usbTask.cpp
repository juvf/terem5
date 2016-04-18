/*
 * usbTask.cpp
 *
 *  Created on: 29 апр. 2015 г.
 *      Author: juvf
 */
#include "usbTask.h"
#include "osConfig.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"

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
return;

	while(1)
	{
//		GPIO_SetBits(GPIOA, GPIO_Pin_10);
//		GPIO_SetBits(GPIOA, GPIO_Pin_9);
//		vTaskDelay(10000);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
		vTaskDelay(10000);
		asm("nop");
		//USART_SendData(USART2, 0x55);
	}
}



