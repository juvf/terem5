/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "stm32f4xx_conf.h"
#include  "../../structCommon.h"
#include  "configTerem.h"
#include "sensor/Sensor.h"
#include "Process.h"

float valueSens[16];

//void taskMeasurement(void *context)
//{ //������ ����������� �� ������� ��� ��������� ����� ��������.
////����� ������ �� ���� ������������ ������� �������� � �������� ��.
////����� ������ ��������.... � ������� ������� ���� ��������� - ������� �� ��� ����� �� ������� � ��� ���
////��������� ����
//
//}

void taskMeasurement(xTimerHandle xTimer)
{
// ��� ������� �������
	//musuring();
	xEventGroupSetBits(xCreatedEventGroup, FLAG_MESUR);
	return;
}

void musuring()
{
	static int a = 0;
	int j = 0;
	float val;
	//�������� ������� ���
	xSemaphoreTake( semaphAdc, portMAX_DELAY );
	ep1_On();
	for(int i = 0; i<8; i++)
	{//����� ���� ������
		switch( configTerem.sensorType[i])
		{
			//������� �����������
			case GT_MM10:
				val = readAnalogSensor(i) * 2.0 / 1.17;
				val = (val - configTerem.a[i][0]) * 5.5;//��������� � ��
				valueSens[j++] = val;
				//valueSens[j++] = 1.2 + 0.15 * j;
				break;
			default:
				break;
		}
	}
	ep1_Off();
	//��������� ������� ���
	xSemaphoreGive(semaphAdc);

	//���������� ���������
	saveResult(valueSens, j);

	if( a == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
//		GPIO_SetBits(GPIOA, GPIO_Pin_9);
		a = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
		a = 0;
	}
}


