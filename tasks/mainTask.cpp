#include "mainTask.h"
#include "../osConfig.h"
#include "stm32f4xx_conf.h"
#include "../i2c.h"
#include "configTerem.h"
#include "taskMeasurement.h"
#include "Process.h"
#include "main.h"
#include "adc.h"
#include "CritSect.h"
#include "sensor/ds1820.h"
#include "sensor/Sensor.h"
#include "taskUartRfd.h"

#include <string.h>

void mainTask(void *context)
{
	xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_WRITE_PARAM);
	ledRedOn();
	initConfigTerem();
	//initListProc();
	ledRedOff();

	while(1)
	{
		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup,
		FLAG_NO_WORK, pdFALSE, pdTRUE, 100);
		if( (uxBits & (FLAG_NO_WORK)) == FLAG_NO_WORK )
			stopJ();
		uxBits = xEventGroupWaitBits(xEventGroup, FLAG_WRITE_PARAM, pdTRUE,
				pdTRUE, 2);
		if( (uxBits & (FLAG_WRITE_PARAM)) == FLAG_WRITE_PARAM )
		{
			xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_WRITE_PARAM);
			saveParam();
		}
	}
}

extern "C" void EXTI3_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line3);
	deinitExti();
	xEventGroupClearBitsFromISR(xEventGroup, FLAG_NO_WORKS_BT);
	initUartRfd();
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ѕри получении
}

extern "C" void EXTI9_5_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line9);
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET )
	{ //питание по€вилось
		xEventGroupClearBitsFromISR(xEventGroup, FLAG_USB_NO_POWER);
		xEventGroupClearBitsFromISR(xEventGroup, FLAG_NO_WORKS_USB);
	}
	else
		xEventGroupSetBitsFromISR(xEventGroup, FLAG_USB_NO_POWER,
				&xHigherPriorityTaskWoken);
}

void initExti()
{
	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	EXTI_ClearFlag(EXTI_Line3);
}

void deinitExti()
{
	EXTI_InitTypeDef exti;
	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = DISABLE;
	EXTI_Init(&exti);

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

}

void stopJ()
{
	enterCritSect();
	pereferDeInit();
	//ledRedOn();
	epa_Off();
	ep1_Off();
	initExti();

	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//ledRedOff();

	/* Disable Wakeup Counter */
	RTC_WakeUpCmd(DISABLE);
	/* After wake-up from STOP reconfigure the system clock */
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
	{
	}

	/* Enable PLL */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		;

	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	while(RCC_GetSYSCLKSource() != 0x08)
		;
	exitCritSect();
	initAfterStop();
}

//костин код
//void TApplication::systemPowerOff(void)
//{
//	TCritSect cs;
//
//    m_evPressPower.clear();
//	/* Enable Wakeup Counter */
////	RTC_WakeUpCmd(ENABLE);
//
//	PWR_WakeUpPinCmd(ENABLE);
//	/* Enter Stop Mode */
////
////NVIC_InitTypeDef  NVIC_InitStructure;
////EXTI_InitTypeDef  EXTI_InitStructure;
/////* EXTI configuration *******************************************************/
////EXTI_ClearITPendingBit(EXTI_Line22);
////EXTI_InitStructure.EXTI_Line = EXTI_Line22;
////EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
////EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
////EXTI_InitStructure.EXTI_LineCmd = ENABLE;
////EXTI_Init(&EXTI_InitStructure);
////
/////* Enable the RTC Wakeup Interrupt */
////NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
////NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
////NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
////NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////NVIC_Init(&NVIC_InitStructure);
////
/////* RTC Wakeup Interrupt Generation: Clock Source: RTCCLK_Div16, Wakeup Time Base: ~20s
//// RTC Clock Source LSI ~32KHz
////
//// Wakeup Time Base = (16 / (LSI)) * WakeUpCounter
////*/
////RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
////RTC_SetWakeUpCounter(0xA000-1);
////
/////* Enable the Wakeup Interrupt */
////RTC_ITConfig(RTC_IT_WUT, ENABLE);
////
/////* Enable Wakeup Counter */
////RTC_WakeUpCmd(ENABLE);
////
/////* FLASH Deep Power Down Mode enabled */
////PWR_FlashPowerDownCmd(ENABLE);
//
//	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
//
//	/* Disable Wakeup Counter */
////	RTC_WakeUpCmd(DISABLE);
//
//	/* After wake-up from STOP reconfigure the system clock */
//	/* Enable HSE */
//	RCC_HSEConfig(RCC_HSE_ON);
//
//	/* Wait till HSE is ready */
//	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
//	{
//	}
//
//	/* Enable PLL */
//	RCC_PLLCmd(ENABLE);
//
//	/* Wait till PLL is ready */
//	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//	{
//	}
//
//	/* Select PLL as system clock source */
//	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
//
//	/* Wait till PLL is used as system clock source */
//	while (RCC_GetSYSCLKSource() != 0x08)
//	{
//	}
//}

