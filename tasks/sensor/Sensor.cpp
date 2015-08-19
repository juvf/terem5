/*
 * Sensor.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "Sensor.h"
#include "../../structCommon.h"
#include "../configTerem.h"

float readSenser(uint8_t numChanel)
{
	float value;
	switch(configTerem.sensorType[numChanel])
	{
		case GT_MM10:
			value = 10.1;
			break;
		case GT_MM20:
			value = 20.2;
			break;
		case GT_MM50:
			value = 50.2;
			break;
		default:
			value = 0;
			break;
	}
	return value;
}

//����� �� 0 �� 7
float readAnalogSensor(uint8_t numChanel)
{
	if(numChanel > 7)
		return -1;
	//������ +500 �� �� 1 ����
	gnd500mVOn();
	//������ 1,67 � �� 4 ����
	powerDa17_16(P_1_67);
	powerDa12_15(numChanel);
	//������������� ���� � �������� ����
	switchOn(numChanel);
	//��������
	float curU = getU_Ad7792(numChanel);
	//��������� ����
	switchOn(100);
	//��������� 1,67 �
	powerDa17_16(P_OFF);
	//��������� 500 ��
	gnd500mVOff();
	return curU;
}

void powerDa17_16(uint8_t val)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
	GPIO_ResetBits(GPIOE, GPIO_Pin_0);
	switch(val)
	{
		case P_1_67:
			GPIO_SetBits(GPIOE, GPIO_Pin_0);
			break;
		case P_ADC_REF:
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			break;
		case P_3_0:
			GPIO_SetBits(GPIOD, GPIO_Pin_4);
			break;
		case P_EP1:
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			break;
	}
}

void switchOn(uint8_t numChanel)
{
	//E0 - PC12, E1 - PB7, A0 - PB5, A1 - PD7
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	switch(numChanel)
	{
		case 0:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 1:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 2:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 3:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 4:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 5:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 6:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 7:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
	}
}

void powerDa12_15(uint8_t val)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_10);
	GPIO_ResetBits(GPIOE, GPIO_Pin_1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	switch(val)
	{
		case 0:
			GPIO_SetBits(GPIOC, GPIO_Pin_11);
			break;
		case 1:
			GPIO_SetBits(GPIOC, GPIO_Pin_10);
			break;
		case 2:
			GPIO_SetBits(GPIOD, GPIO_Pin_1);
			break;
		case 3:
			GPIO_SetBits(GPIOD, GPIO_Pin_0);
			break;
		case 4:
			GPIO_SetBits(GPIOB, GPIO_Pin_6);
			break;
		case 5:
			GPIO_SetBits(GPIOD, GPIO_Pin_6);
			break;
		case 6:
			GPIO_SetBits(GPIOE, GPIO_Pin_1);
			break;
		case 7:
			GPIO_SetBits(GPIOD, GPIO_Pin_5);
			break;

	}
}

