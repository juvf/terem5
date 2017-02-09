/*
 * clock.cpp
 *
 *  Created on: 25 июл€ 2015 г.
 *      Author: juvf
 */
#include "clock.h"
#include "Process.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_conf.h"
#include "system_stm32f4xx.h"

void initRtc()
{
	//http://microtechnics.ru/stm32f4-chasy-realnogo-vremeni-rtc/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //¬ключаем тактирование (PWR Ч Power Control):

	PWR_BackupAccessCmd(ENABLE);

	//RCC_BackupResetCmd(ENABLE);
	//RCC_BackupResetCmd(DISABLE);

	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		;
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//	RCC_LSICmd(ENABLE);
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);

	RTC_InitTypeDef rtc;
	rtc.RTC_HourFormat = RTC_HourFormat_24;
	rtc.RTC_SynchPrediv = 0x7FFF;
	rtc.RTC_AsynchPrediv = 0;

	RTC_WaitForSynchro();
	RTC_Init(&rtc);

	RTC_BypassShadowCmd(ENABLE);

	RTC_ClearFlag(RTC_FLAG_ALRAF);

	EXTI_InitTypeDef exti;
	EXTI_ClearITPendingBit(EXTI_Line17);
	exti.EXTI_Line = EXTI_Line17;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 13);
}

//заводит будильник Alarm_A на врем€ через seconds секунд
void setNewAlarmRTC(uint32_t seconds)
{

	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);

	RTC_TimeTypeDef alarmTime;
	RTC_GetTime(RTC_Format_BIN, &alarmTime);
	addSecToTime(&alarmTime, seconds);

	RTC_AlarmTypeDef alarm;
	RTC_AlarmStructInit(&alarm);
	alarm.RTC_AlarmTime = alarmTime;
	alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm);

	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

//прибавл€ет к времени time секунды seconds и помещ€ет полученное врем€ в time
bool addSecToTime(RTC_TimeTypeDef *time, uint32_t seconds)
{
	time->RTC_Seconds += seconds % 60;
	if( time->RTC_Seconds > 59 )
	{
		time->RTC_Seconds -= 60;
		time->RTC_Minutes += 1;
	}
	time->RTC_Minutes += (seconds / 60) % 60;
	if( time->RTC_Minutes > 59 )
	{
		time->RTC_Minutes -= 60;
		time->RTC_Hours += 1;
	}
	time->RTC_Hours += (seconds / (60 * 60)) % 24;
	if( time->RTC_Hours > 23 )
	{
		time->RTC_Hours -= 24;
	}
	return true;
}

bool setRtcTime(uint8_t *buffer)
{
	if( (stateProcess == 2) || (stateProcess == 0) )
	{
		RTC_TimeTypeDef time;
		time.RTC_Hours = buffer[2];
		time.RTC_Minutes = buffer[1];
		time.RTC_Seconds = buffer[0];

		if( RTC_SetTime(RTC_Format_BIN, &time) == SUCCESS )
		{
			RTC_DateTypeDef date;
			RTC_DateStructInit(&date);
			date.RTC_Date = buffer[3];
			date.RTC_Month = buffer[4];
			date.RTC_Year = buffer[5];
			if( RTC_SetDate(RTC_Format_BIN, &date) == SUCCESS )
				return true;
		}
	}
	return false;
}

void getRtcTime(uint8_t *buffer)
{
	RTC_TimeTypeDef time;
	RTC_GetTime(RTC_Format_BIN, &time);
	RTC_DateTypeDef date;
	RTC_GetDate(RTC_Format_BIN, &date);

	buffer[0] = time.RTC_Seconds;
	buffer[1] = time.RTC_Minutes;
	buffer[2] = time.RTC_Hours;
	buffer[3] = date.RTC_Date;
	buffer[4] = date.RTC_Month;
	buffer[5] = date.RTC_Year;
}

