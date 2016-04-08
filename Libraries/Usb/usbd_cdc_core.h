/**
  ******************************************************************************
  * @file    usbd_cdc_core.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   header file for the usbd_cdc_core.c file.
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

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_CDC_CORE_H_
#define __USB_CDC_CORE_H_

#include  "usbd_ioreq.h"

#define USB_CDC_CONFIG_DESC_SIZ                (67)
#define USB_CDC_DESC_SIZ                       (67-9)

#define DEVICE_CLASS_CDC                        0x02
#define DEVICE_SUBCLASS_CDC                     0x00


#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

#define CDC_DATA_IN_PACKET_SIZE                CDC_DATA_MAX_PACKET_SIZE
        
#define CDC_DATA_OUT_PACKET_SIZE               CDC_DATA_MAX_PACKET_SIZE



typedef struct _CDC_IF_PROP
{
  uint16_t (*pIf_DataTx)   (void);
  uint16_t (*pIf_DataRx)   (uint8_t* Buf, uint32_t Len, void *pdev);
}
CDC_IF_Prop_TypeDef;

extern USBD_Class_cb_TypeDef  USBD_CDC_cb;

#endif /* __USB_CDC_CORE_H_ */

  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
