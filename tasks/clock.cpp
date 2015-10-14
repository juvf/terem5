/*
 * clock.cpp
 *
 *  Created on: 25 ���� 2015 �.
 *      Author: juvf
 */
#include "clock.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_conf.h"
#include "system_stm32f4xx.h"

void initRtc()
{
	//http://microtechnics.ru/stm32f4-chasy-realnogo-vremeni-rtc/
	RTC_InitTypeDef rtc;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //�������� ������������ (PWR � Power Control):

	PWR_BackupAccessCmd(ENABLE);

	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	RCC_LSEConfig(RCC_LSE_ON);
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);

	rtc.RTC_HourFormat = RTC_HourFormat_24;
	rtc.RTC_SynchPrediv = 0x7FFF;
	RTC_Init(&rtc);

	EXTI_InitTypeDef exti;
	EXTI_ClearITPendingBit(EXTI_Line17);
	exti.EXTI_Line = EXTI_Line17;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 13);

//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
}
//RTC_TimeTypeDef asd[10][3];
//int ij = 0;
//������� ��������� Alarm_A �� ����� ����� seconds ������
void setNewAlarmRTC(uint32_t seconds)
{

	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);

	for(int i = 0; i<1000; i++)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}

	RTC_TimeTypeDef time;
	RTC_GetTime(RTC_Format_BIN, &time);


	RTC_AlarmTypeDef alarm1;
	//RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm1);
	//asd[ij][2] = alarm1.RTC_AlarmTime;

	RTC_TimeTypeDef alarmTime = time;
	addSecToTime(&alarmTime, seconds);
	//asd[ij][0] = time;
	//if(ij == 10)
	//	ij--;

	RTC_AlarmTypeDef alarm;
	alarm.RTC_AlarmTime = alarmTime;
	//asd[ij++][1] = alarm.RTC_AlarmTime;
	alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alarm);

	//RTC_OutputConfig(RTC_Output_AlarmA, RTC_OutputPolarity_High);
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	RTC_ClearFlag(RTC_FLAG_ALRAF);
}

//���������� � ������� time ������� seconds � �������� ���������� ����� � time
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
	RTC_TimeTypeDef time;
	RTC_TimeStructInit(&time);
	RTC_DateTypeDef date;
	RTC_GetDate(RTC_Format_BIN, &date);

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

