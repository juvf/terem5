#include "mainTask.h"
#include "../osConfig.h"
#include "stm32f4xx_conf.h"
#include "../i2c.h"
#include "taskMeasurement.h"

void mainTask(void *context)
{
	char *str;
	while(1)
	{
          vTaskDelay(1000);
          //musuring();


      	//тестируем и2ц
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
//		{//вывод сообщения в порт
//			while( *str != 0)
//			{
//				vTaskDelay(1);
//				USART_SendData(USART2, *str);
////				while(USART_GetITStatus(USART2, USART_IT_TC) == SET);//ждём освобождения от предыдущей отправки
//				//отправить в порт
//				str++;
//			}
//			vTaskDelay(1);
//			USART_SendData(USART2, '\n');//отправить в порт \n
////			while(USART_GetITStatus(USART2, USART_IT_TC) == SET)
////				;				//ждём освобождения от предыдущей отправки
//		}
                //else
	}
}


