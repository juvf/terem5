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
#include "sensor/Dpg2.h"
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
		pdFALSE, pdTRUE, 3000);
		if( (uxBits & FLAG_MESUR) == FLAG_MESUR )
		{
			ledGreenOn();
			setNewAlarmRTC(getProcessPeriod()); 		//������������ ������
			musuring();
			vTaskDelay(300);
			ledGreenOff();
			//xEventGroupSetBits(xEventGroup, FLAG_SLEEP_MESUR);
			xEventGroupClearBits(xEventGroup, FLAG_MESUR);
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
	static ResultMes result1;
	int j = 0;
	//�������� ������� ���
	xSemaphoreTake(semaphAdc, portMAX_DELAY);
	//�������� ���� ���������� ����� ������
	tempOfDs1820 = readtemp() + configTerem.deltaT;

	ep1_On();
	epa_On();
	for(int i = 0; i < 9; i++)
	{	//����� ���� �������
		result1 = readSenser(i);
		if( isnan(result1.u) == 0 )
		{
			if( configTerem.sensorType[i] == GT_SHT21 )
				valueSens[j++] = result1.uClear;
			else
				valueSens[j++] = result1.p;
		}
//		if((configTerem.Flags & 8) > 0)
//		{//������ ��� 2.0
//			dpg2_readValue(8, &result1);
//			valueSens[j++] = result1.uClear;
//		}
	}
	epa_Off();
	ep1_Off();

	xEventGroupSetBits(xEventGroup, FLAG_IS_READY_MES);
	//��������� ������� ���
	xSemaphoreGive(semaphAdc);

	//���������� ���������
	saveResult(valueSens, j);
}

