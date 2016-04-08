#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED 
#pragma     data_alignment = 4 
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "osConfig.h"



USART_InitTypeDef USART_InitStructure;

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t VCP_DataTx   (void);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len, void *pdev);

static uint16_t VCP_COMConfig(uint8_t Conf);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_DataTx,
  VCP_DataRx
};
static uint16_t VCP_DataTx (void)
{
  return USBD_OK;
}

static uint16_t VCP_DataRx (uint8_t* buffer, uint32_t Len, void *pdev)
{
  uint32_t command = *(uint32_t*)buffer;
  char *mess;
  switch(command)
  {
  case 02:
    mess = "Get command ReadName";
    xQueueSendFromISR(cansolQueue, &mess, 0);
    
    DCD_EP_Tx( pdev, 02, "Hello", 6);
   
      
    break;
  }
 
  return USBD_OK;
}





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
