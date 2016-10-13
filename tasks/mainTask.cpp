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

#define FLAG_SLEEP	(FLAG_SLEEP_UART | FLAG_SLEEP_MESUR)
void mainTask(void *context)
{
	ledRedOn()
	;
	initConfigTerem();
	initListProc();
	ledRedOff()
	;

	while(1)
	{
		//ledGreenOn();
		//ждем флагов чтобы уйти в режим микропотребления, в Stop Mode
		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, FLAG_SLEEP | FLAG_WRITE_PARAM,
		pdTRUE, pdTRUE, 1000);
		//ledGreenOff();
		if( (uxBits & FLAG_SLEEP) == FLAG_SLEEP )
			sleepJ();
		if( (uxBits & FLAG_WRITE_PARAM) == FLAG_WRITE_PARAM )
			saveParam();

		vTaskDelay(1000);
	}
}

extern "C" void EXTI3_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line3);
	//flagExti = 0;
	deinitExti();
	//ledGreenOn();
}

void initExti()
{
	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;

	EXTI_ClearFlag(EXTI_Line3);

	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

}

void deinitExti()
{
	EXTI_ClearFlag(EXTI_Line3);
//	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
//	gpio.GPIO_Mode = GPIO_Mode_IN;
//	gpio.GPIO_Pin = GPIO_Pin_3;
//	gpio.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpio);

	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = DISABLE;
	EXTI_Init(&exti);

	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&nvic);
}

void sleepJ()
{
	ledRedOn();
	vTaskDelay(200);
	return;
	enterCritSect();
	ledGreenOff();
	pereferDeInit();
	exitCritSect();
	return;
	initExti();
	vTaskDelay(300);
	//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
//	while(flagExti)

	/* Disable Wakeup Counter */
	//	RTC_WakeUpCmd(DISABLE);
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
	{
	}

	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
//	EXTI_ClearFlag(EXTI_Line3);
//	deinitExti();
	pereferInit();
	//ledGreenOn();
	exitCritSect();
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

