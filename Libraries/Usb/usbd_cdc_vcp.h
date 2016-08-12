/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   Header for usbd_cdc_vcp.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core.h"
#include "usbd_conf.h"
#endif /* __USBD_CDC_VCP_H */

#define COM_USB_SEND_MESSAGE	3 //от компа
#define COM_USB_GET_MESSAGE		4 //получить компу
#define COM_USB_READ_MEM		5 //чтение памяти
#define COM_USB_READ_ADDRESS	6 //чтение адреса
#define COM_USB_WRITE_ADDRESS	7 //запись адреса
#define COM_USB_RF_COM			8 //команда по протоколу радиоканала



void usbReplayGetMessage(void *pdev);
void usbSenMessToWT41(uint8_t *buf, uint32_t Len);
void usbReadMemory(uint8_t* buffer, void *pdev);
void usbReadAddress(uint8_t* buffer, void *pdev);
void usbWriteAddress(uint8_t* buffer, void *pdev);
void usbRfComand(uint8_t* buffer, void *pdev, uint32_t Len);


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
