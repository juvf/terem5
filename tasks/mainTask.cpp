#include "mainTask.h"
#include "../osConfig.h"
#include "stm32f4xx_conf.h"

void mainTask(void *context)
{
	char *str;
	while(1)
	{
          vTaskDelay(1000);
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
                  asm("nop");
	}
}


