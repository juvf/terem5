/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "stm32f4xx_conf.h"

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
	musuring();
	return;
}

void musuring()
{
	static int a = 0;

	if( a == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
		GPIO_SetBits(GPIOA, GPIO_Pin_9);
		a = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
		a = 0;
	}
}

