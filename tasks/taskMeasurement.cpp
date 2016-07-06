/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "clock.h"
#include "stm32f4xx_rtc.h"
#include  "../../structCommon.h"
#include  "configTerem.h"
#include "sensor/Sensor.h"
#include "Process.h"
#include "../../adc.h"
#include "sensor/ds1820.h"
#include "main.h"
#include <math.h>

float valueSens[16];

void taskMeasurement(void *context)
{ //������ ����������� �� ������� ��� ��������� ����� ��������.
//����� ������ �� ���� ������������ ������� �������� � �������� ��.
//����� ������ ��������.... � ������� ������� ���� ��������� - ������� �� ��� ����� �� ������� � ��� ���
//��������� ����
	vTaskDelay(1000);
	while(1)
	{
		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, FLAG_MESUR,
		pdTRUE, pdTRUE, 3000);
		if( (uxBits & FLAG_MESUR) == FLAG_MESUR )
		{
			ledRedOn();
			setNewAlarmRTC(getProcessPeriod()); 		//������������ ������
			musuring();
			vTaskDelay(300);
			ledRedOff();

		}
		else
		{
			xEventGroupSetBits(xEventGroup, FLAG_SLEEP_MESUR);
		}
	}

}

void timerMeasurement(xTimerHandle xTimer)
{
// ��� ������� �������
	//musuring();
	//xEventGroupSetBits(xEventGroup, FLAG_MESUR);
	//return;
}

void musuring()
{
	int j = 0;
	//�������� ������� ���
	xSemaphoreTake(semaphAdc, portMAX_DELAY);
	//�������� ���� ���������� ����� ������
	tempOfDs1820 = readtemp();
	ep1_On();
	epa_On();
	for(int i = 0; i < 8; i++)
	{	//����� ���� ������
		ResultMes result = readSenser(i);
		if( isnan(result.u) == 0 )
			valueSens[j++] = result.p;
	}
	if( j != 4)
		asm("nop");
	epa_Off();
	ep1_Off();
	//��������� ������� ���
	xEventGroupSetBits(xEventGroup, FLAG_IS_READY_MES);
	xSemaphoreGive(semaphAdc);

	if((valueSens[0] > 50) || (valueSens[0] < 19))
		asm("nop");

	//���������� ���������
	saveResult(valueSens, j);
}

