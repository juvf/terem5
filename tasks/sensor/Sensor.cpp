/*
 * Sensor.cpp
 *
 *  Created on: 06 авг. 2015 г.
 *      Author: anisimov.e
 */

#include "Sensor.h"
#include "../../structCommon.h"
#include "../configTerem.h"
#include "../../adc.h"
#include "SensorM10.h"
#include "GaugeHK.h"
#include "GaugeHA.h"
#include "GaugeT48.h"
#include "GaugeHEL700.h"
#include "GaugeHeatFlow.h"
#include <math.h>

//температура холодного спая
#define  TEMP_HS	22

//канал от 0 до 7
float readAnalogSensor(uint8_t numChanel)
{
	if(numChanel > 7)
		return -1;
	//подать +500 мВ на 1 ногу
	gnd500mVOn();
	//подать 1,67 В на 4 ногу
	powerDa17_16(P_1_67);
	powerDa12_15(numChanel);
//	//скомутировать ключ и включить ключ
//	switchOn(numChanel);
	//измерить
	float curU = getU_Ad7792(numChanel);
//	//выключить ключ
//	switchOn(100);
	//выключить 1,67 В
	powerDa17_16(P_OFF);
	//выключить 500 мВ
	gnd500mVOff();
	return curU;
}

void powerDa17_16(uint8_t val)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	switch(val)
	{
		case P_1_67:
			GPIO_SetBits(GPIOD, GPIO_Pin_5);
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
	GPIO_ResetBits(GPIOD, GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_10);
	GPIO_ResetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_0);
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
			GPIO_SetBits(GPIOE, GPIO_Pin_0);
			break;

	}
}

//Полином четвертой степени ---------------------------------------------------
//Вход: аргумент, указатель на коэффициенты (начиная с A4)
float Polinom4(float x, const float *A)
{
	float f = A[0] * x;          //A4
	f += A[1];
	f *= x;          //A3
	f += A[2];
	f *= x;          //A2
	f += A[3];
	f *= x;          //A1
	f += A[4];       //A0
	return f;
}

//Полином третьей степени -----------------------------------------------------
//Вход: аргумент, указатель на коэффициенты (начиная с A3)
float Polinom3(float x, const float *A)
{
	float f = A[0] * x;          //A3
	f += A[1];
	f *= x; //A2
	f += A[2];
	f *= x; //A1
	f += A[3];         //A0
	return f;
}

//
ResultMes readSenser(uint8_t numChanel)
{
	ResultMes result;
	switch(configTerem.sensorType[numChanel])
	{
		//Датчики перемещения
		case GT_MM10:
			result.u = readAnalogSensor(numChanel);
			result.p = MM10_Length(result.u, configTerem.a[numChanel][0]);
			//				valP = valU * 2.0 / 1.17;
			//				valP = (valP - configTerem.a[buffer[6]][0]) * 5.5; //Результат в мм
			break;
		case GT_MM20:
			result.u = readAnalogSensor(numChanel);
			result.p = MM20_Length(result.u, configTerem.a[numChanel][0]);
			break;
		case GT_MM50:
			result.u = readAnalogSensor(numChanel);
			result.p = MM50_Length(result.u, configTerem.a[numChanel][0]);
			break;
		case GT_HEL700:			//Платиновый ТСП -> в градусах
			result.u = getU_Ad7792(numChanel);
			result.p = HEL700_Termo(result.u, numChanel);
			break;
		case GT_TermoHK:			//Термопара ХК -> в градусах
		case GT_TermoHKcom:
			result.u = getU_Ad7792(numChanel);
			result.p = HK_Termo(result.u, TEMP_HS);
			break;
		case GT_TermoHA:			//Термопара ХА -> в градусах
		case GT_TermoHAcom:
			result.u = getU_Ad7792(numChanel);
			result.p = HA_Termo(result.u, TEMP_HS);
			break;
		case GT_Termo48:			//D Универсальный термопарный вход
			result.u = getU_Ad7792(numChanel);
			result.p = T48_Termo(result.u, TEMP_HS, numChanel);
			break;
		case GT_HeatFlowPeltje:
			result.u = getU_Ad7792(numChanel);
			result.p = HF_Flow(result.u, koeffsAB.koef[numChanel].a);
			break;
		case GT_HeatFlowPeltje48:
			result.u = getU_Ad7792(numChanel);
			result.p = HF_Flow48(result.u, numChanel);
			break;
		default:
			result.u = NAN;
			result.p = NAN;
			break;
	}
	return result;
}
