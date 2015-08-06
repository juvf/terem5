/*
 * Process.cpp
 *
 *  Created on: 05 ���. 2015 �.
 *      Author: juvf
 */
#include "Process.h"
#include "stm32f4xx_rtc.h"
#include "../osConfig.h"
#include "taskMeasurement.h"

uint8_t stateProcess; //������� ��������� �������� 0-��� ��������, 1-��� �������, 2-����������, 3-���� ������
Process currProcessHeader; //��������� �������� ��������
uint32_t currProcessCount; //���-�� ���������� �����

int commandStartProc(uint8_t *buffer)
{
	if((stateProcess == 0) || (stateProcess == 2))
	{
		uint32_t per = (buffer[9] << 24) | (buffer[8] << 16) | (buffer[7] << 8) | buffer[6];
		if((per > 0) && (per < 4294960))
		{
			currProcessHeader.period = per;
			if(buffer[0] == 0xff)
			{ //����������� ������ �������
				musuring();
				if ( xTimerChangePeriod(timerMesuring, per * 1000, 100) == pdFAIL )
					buffer[0] = 1;
				else
				{
					xTimerReset(timerMesuring, 100);
					buffer[0] = 0;
				}
			}
			else
			{
				currProcessHeader.startTime.RTC_Seconds = buffer[0];
				currProcessHeader.startTime.RTC_Minutes = buffer[1];
				currProcessHeader.startTime.RTC_Hours = buffer[2];
				currProcessHeader.startDate.RTC_Date = buffer[3];
				currProcessHeader.startDate.RTC_Month = buffer[4];
				currProcessHeader.startDate.RTC_Year = buffer[5];
				buffer[0] = 0;
			}
		}
		else
			buffer[0] = 1;
	}
	else
	{ //������ ��� ���, ��� ��� ������, ������� ������ ������ ��������� ��������
		buffer[0] = 1;
	}
	return 7;
}

int commandGetProcConf(uint8_t *buffer)
{
	buffer[0] = currProcessHeader.startTime.RTC_Seconds;
	buffer[1] = currProcessHeader.startTime.RTC_Minutes;
	buffer[2] = currProcessHeader.startTime.RTC_Hours;
	buffer[3] = currProcessHeader.startDate.RTC_Date;
	buffer[4] = currProcessHeader.startDate.RTC_Month;
	buffer[5] = currProcessHeader.startDate.RTC_Year;
	buffer[6] = currProcessHeader.period & 0xff;
	buffer[7] = currProcessHeader.period >> 8;
	return 14;
}

int commandStopProc(uint8_t *buffer)
{

}
