#include "mainTask.h"
#include "../osConfig.h"
#include "stm32f4xx_conf.h"
#include "../i2c.h"
#include "configTerem.h"
#include "taskMeasurement.h"
#include "Process.h"

void mainTask(void *context)
{
	char *str;

	initConfigTerem();
	initListProc();
	while(1)
	{
		vTaskDelay(1000);
		EventBits_t uxBits = xEventGroupWaitBits(xCreatedEventGroup, FLAG_MESUR,
				pdTRUE, pdTRUE, 100);
		if((uxBits & FLAG_MESUR) == FLAG_MESUR)
			musuring();

		//��������� �2�
//          static uint8_t bbb[] = {1,2,3,4,5,6};
//          i2cWrite(0xA0, 0, bbb+2, 3);
//          vTaskDelay(100);
//          static uint8_t aaa[10] = {0};
//          for(int i = 0; i < 5; i++)
//        	  aaa[i] = 99;
//          i2cRead(0xa0, 0, aaa, 3);
//                  asm("nop");

//          GPIO_SetBits(GPIOD, GPIO_Pin_13);
//          vTaskDelay(1000);
//          GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//		if( xQueueReceive(cansolQueue, (void*)&str, portMAX_DELAY) == pdPASS)
//		{//����� ��������� � ����
//			while( *str != 0)
//			{
//				vTaskDelay(1);
//				USART_SendData(USART2, *str);
////				while(USART_GetITStatus(USART2, USART_IT_TC) == SET);//��� ������������ �� ���������� ��������
//				//��������� � ����
//				str++;
//			}
//			vTaskDelay(1);
//			USART_SendData(USART2, '\n');//��������� � ���� \n
////			while(USART_GetITStatus(USART2, USART_IT_TC) == SET)
////				;				//��� ������������ �� ���������� ��������
//		}
		//else
	}
}

