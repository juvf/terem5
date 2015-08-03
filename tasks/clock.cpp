/*
 * clock.cpp
 *
 *  Created on: 25 июл€ 2015 г.
 *      Author: juvf
 */
#include "clock.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_conf.h"
#include "system_stm32f4xx.h"

void initRtc()
{
	//http://microtechnics.ru/stm32f4-chasy-realnogo-vremeni-rtc/
	RTC_InitTypeDef rtc;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //¬ключаем тактирование (PWR Ч Power Control):

	PWR_BackupAccessCmd(ENABLE);

	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	RCC_LSICmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);

	rtc.RTC_HourFormat = RTC_HourFormat_24;
	rtc.RTC_SynchPrediv = 0x7FFF;
	RTC_Init(&rtc);

}

bool setRtcTime(uint8_t *buffer)
{
	RTC_TimeTypeDef time;
	RTC_TimeStructInit(&time);
	time.RTC_Hours = buffer[2];
	time.RTC_Minutes = buffer[1];
	time.RTC_Seconds = buffer[0];

	if(RTC_SetTime(RTC_Format_BIN, &time) == SUCCESS)
	{
		RTC_DateTypeDef date;
		RTC_DateStructInit(&date);
		date.RTC_Date = buffer[3];
		date.RTC_Month = buffer[4];
		date.RTC_Year = buffer[5];
		if(RTC_SetDate(RTC_Format_BIN, &date) == SUCCESS)
			return true;
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

