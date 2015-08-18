/*
 * SensorM10.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "SensorM10.h"

SensorM10::SensorM10(uint8_t num) :
		Sensor(num)
{
}

SensorM10::~SensorM10()
{
}

float SensorM10::read()
{
	return 1.0;
}

//����� �� 0 �� 7
float readM(uint8_t numChanel)
{
	if(numChanel > 7)
		return -1;
	//������ +500 �� �� 1 ����
	gnd500mVOn();
	//������ 1,67 � �� 4 ����
	powerDa17_16(P_1_67);
	powerDa12_15(numChanel);
	//������������� ����

	//�������� ����
	//��������
	//��������� ����
	//��������� 1,67 �
	powerDa17_16(P_OFF);
	//��������� 500 ��
	gnd500mVOff();
	return 1.0;
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
