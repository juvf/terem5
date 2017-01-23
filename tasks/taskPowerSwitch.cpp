/*
 * taskPowerSwitch.cpp
 *
 *  Created on: 5 янв. 2017 г.
 *      Author: anisimov.e
 */

#include "taskPowerSwitch.h"
#include "../osConfig.h"
#include "../main.h"
#include "Process.h"

#include "stm32f4xx_conf.h"

void taskPowerSwitch(void *context)
{

	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	EXTI_ClearFlag(EXTI_Line0);

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_SetPriority(EXTI0_IRQn, 12);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	ledRedOff();
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET)
		vTaskDelay(2);
	vTaskDelay(63);
	ledRedOn();
	vTaskDelay(37);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	vTaskDelay(20);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	vTaskDelay(6);
	ledRedOff();

	for(int i = 0; i < 4; i++)
	{
		vTaskDelay(63);
		ledRedOn();
		vTaskDelay(63);
		ledRedOff();
	}

	xEventGroupClearBits(xEventGroup, FLAG_SW_POWER);
	while(1)
	{
		xEventGroupWaitBits(xEventGroup, FLAG_SW_POWER, pdTRUE, pdTRUE,
		portMAX_DELAY);
		ledRedOff();
		//защита от антидребезга
		uint8_t port = 0xFF;
		while(port)
		{
			vTaskDelay(8);
			port <<= 1;
			port |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		}


		if( stateProcess == 1 )
		{ //не выключать, идет процесс
			vTaskDelay(100 - 64);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			vTaskDelay(50);
			GPIO_SetBits(GPIOB, GPIO_Pin_12);
			vTaskDelay(100);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			vTaskDelay(20);
			GPIO_SetBits(GPIOB, GPIO_Pin_12);
		}
		else
		{ //выключаем прибор
//		vTaskSuspendAll();
			ledRedOn();
			vTaskDelay(250); //63
			ledRedOff();
			vTaskDelay(250); //100
			ledRedOn();
			vTaskDelay(250); //100
			ledRedOff();
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			for(;;)
				vTaskDelay(1); //250
			;
		}
	}
}

extern "C" void EXTI0_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line0);
	xEventGroupSetBitsFromISR(xEventGroup, FLAG_SW_POWER, 0);
}

