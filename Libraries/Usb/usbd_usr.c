#include "usbd_usr.h"
#include "osConfig.h"

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

void USBD_USR_Init(void)
{  
  /* Initialize LEDs */
  //STM_EVAL_LEDInit(LED1);
  //STM_EVAL_LEDInit(LED2);
  //STM_EVAL_LEDInit(LED3);
  //STM_EVAL_LEDInit(LED4);   
  
  /* Initialize the LCD */
#if defined (USE_STM322xG_EVAL)
///  STM322xG_LCD_Init();
#elif defined(USE_STM324xG_EVAL)
 /// STM324xG_LCD_Init();
  
#elif defined(USE_STM324x9I_EVAL)
  
 /// LCD_Init();
  ///LCD_LayerInit();
  
  /* Enable The Display */
  ///LCD_DisplayOn(); 
  /* Connect the Output Buffer to LCD Background Layer  */
  ///LCD_SetLayer(LCD_FOREGROUND_LAYER);
  
  /* Clear the Background Layer */ 
 /// LCD_Clear(LCD_COLOR_WHITE);
  
#endif

  ///LCD_LOG_Init();
  
#ifdef USE_USB_OTG_HS 
  ///LCD_LOG_SetHeader((uint8_t*)" USB OTG HS VCP Device");
#else
  ///LCD_LOG_SetHeader((uint8_t*)" USB OTG FS VCP Device");
#endif
  ///LCD_UsrLog("> USB device library started.\n"); 
  ///LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0" );
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
  char *mess;
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
    /// LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [HS]" );
     break;

  case USB_OTG_SPEED_FULL: 
     ///LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [FS]" );
    mess = "     USB Device Library v1.2.0 [FS]";
    xQueueSendFromISR(cansolQueue, &mess, 0);
     break;
 default:
     ///LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [??]" );
 }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured (void)
{
  ///LCD_UsrLog("> VCP Interface configured.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  ///LCD_UsrLog("> USB Device in Suspend Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
  ///LCD_UsrLog("> USB Device in Idle Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected (void)
{
  ///LCD_UsrLog("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected (void)
{
  ///LCD_UsrLog("> USB Device Disconnected.\n");
}
/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
