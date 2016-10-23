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

#include <string.h>

#define FLAG_STOP	(FLAG_BT_CONNECTED | FLAG_MESUR | FLAG_WRITE_PARAM | FLAG_USB_POWER)
void mainTask(void *context)
{
	ledRedOn();
	initConfigTerem();
	initListProc();
	ledRedOff();

	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET)
		xEventGroupSetBits(xEventGroup, FLAG_USB_POWER);

	if( (xEventGroupGetBits(xEventGroup) & FLAG_BT_CONNECTED) == 0 )
		sleepBt();

	while(1)
	{
		//ledGreenOn();
		//ждем флагов чтобы уйти в режим микропотребления, в Stop Mode
		xEventGroupWaitBits(xEventGroup, FLAG_SLEEP_UART | FLAG_WRITE_PARAM,
		pdFALSE, pdFALSE, 100);
		//ledGreenOff();
		EventBits_t uxBits = xEventGroupGetBits(xEventGroup);
		if( (uxBits & FLAG_SLEEP_UART) == FLAG_SLEEP_UART )
		{
			sleepBt();
		}
		stopJ();

	}
}

extern "C" void EXTI3_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line3);
	deinitExti();
}

extern "C" void EXTI9_5_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line9);
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET )
	{
		xEventGroupSetBitsFromISR(xEventGroup, FLAG_USB_POWER,
				&xHigherPriorityTaskWoken);
		deinitExti();
	}
	else
		xEventGroupClearBitsFromISR(xEventGroup, FLAG_USB_POWER);

}

void initExti()
{
	GPIO_InitTypeDef gpio;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;

//	EXTI_ClearFlag(EXTI_Line3);
//	EXTI_ClearFlag(EXTI_Line3 | EXTI_Line9);

	//RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);

	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

//	exti.EXTI_Line = EXTI_Line9;
//	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	exti.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&exti);

	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

//	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
//	nvic.NVIC_IRQChannelSubPriority = 1;
//	NVIC_Init(&nvic);

	//настроим выход по УСБ подключению.
}

void deinitExti()
{
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = EXTI3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&nvic);
}

extern int endTransmit;
extern uint8_t rfd_buffer[];
extern uint16_t *rfd_count;
extern uint8_t rfd_sizeOfFrame; //длинна пакета

void sleepBt()
{
	//проверим, чтоб не было стирание флешки
	EventBits_t flags = xEventGroupGetBits(xEventGroup);
	if( flags & FLAG_FLASH_CLEARING )
		return;
	else
	{
		xEventGroupClearBits(xEventGroup, FLAG_SLEEP_UART);
		vTaskDelay(100);
		char *mes = "SLEEP\r\n";
		rfd_sizeOfFrame = strlen(mes);
		strcpy((char*)rfd_buffer, mes);
		endTransmit = 1;
		rfd_count = 0;
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		USART_SendData(USART2, rfd_buffer[0]);
		while(endTransmit != 0)
			vTaskDelay(1);
		vTaskDelay(500);
	}
}
void stopJ()
{
	//проверим чтобы не было соединений по УСБ,БТ и не было измерения.
	EventBits_t uxBits = xEventGroupGetBits(xEventGroup);
	if( (uxBits & FLAG_STOP) == 0 )
	{
		enterCritSect();
		pereferDeInit();
		epa_Off();
		ep1_Off();
		initExti();
		ledRedOn();
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		ledRedOff();
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
		EXTI_ClearFlag(EXTI_Line3 | EXTI_Line9);
		exitCritSect();
		initAfterStop();
	}
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

