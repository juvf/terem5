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

#define FLAG_STOP	(FLAG_BT_CONNECTED | FLAG_MESUR | FLAG_WRITE_PARAM | FLAG_USB_NO_POWER)
void mainTask(void *context)
{
	ledRedOn();
	initConfigTerem();
	//initListProc();
	ledRedOff();

	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET )
		xEventGroupClearBits(xEventGroup, FLAG_USB_NO_POWER);

	if( (xEventGroupGetBits(xEventGroup) & FLAG_BT_CONNECTED) == 0 )
		sleepBt();

	while(1)
	{
		//vTaskDelay(3000);
		//ждем флагов чтобы уйти в режим микропотребления, в Stop Mode
		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup,
		FLAG_SLEEP_UART | FLAG_USB_SLEEP,
		pdFALSE, pdTRUE, 100);
		if( (uxBits & (FLAG_SLEEP_UART | FLAG_SLEEP_USB)) != 0 )
		{
			stopJ();
			//initUartRfd();
			//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // При получении
		}

	}
}

extern "C" void EXTI3_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line3);
	deinitExti();
	initUartRfd();
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // При получении
	ledRedOff();
}

extern "C" void EXTI9_5_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line9);
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_SET )
	{
		xEventGroupClearBitsFromISR(xEventGroup, FLAG_USB_NO_POWER | FLAG_SLEEP_USB);
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

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);

	exti.EXTI_Line = EXTI_Line3;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	EXTI_ClearFlag(EXTI_Line3);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_SetPriority(EXTI3_IRQn, 1);

	initPa9();
}

void deinitExti()
{
	NVIC_DisableIRQ(EXTI3_IRQn);

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
	xEventGroupClearBits(xEventGroup, FLAG_SLEEP_UART);
	EventBits_t uxBits = xEventGroupGetBits(xEventGroup);
	if( (uxBits & FLAG_STOP) == 0 )
	{
		NVIC_ClearPendingIRQ(EXTI3_IRQn);
		enterCritSect();
		pereferDeInit();
		ledRedOn();
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

