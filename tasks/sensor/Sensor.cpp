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
#include "ds1820.h"
#include "SensorM10.h"
#include "GaugeHK.h"
#include "GaugeHA.h"
#include "GaugeT48.h"
#include "GaugeHEL700.h"
#include "GaugeHeatFlow.h"
#include "GaugeHIH3610.h"
#include "GaugeTenso.h"
#include "Dpg2.h"
#include "../osConfig.h"
#include "main.h"
#include <math.h>

//температура холодного спая
//#define  TEMP_HS	22

//канал от 0 до 7
float readAnalogSensor(uint8_t numChanel, uint16_t *codeN)
{
	ledRedOn();

	if( numChanel > 7 )
		return -1;
	//подать +500 мВ на 1 ногу
//	gnd500mVOn(); //подать +500 мВ на 1 ногу
	gnd500mVOff();
	//подать 1,67 В на 4 ногу и на АЦП на Vref
	powerDa17_16(P_1_67_V_REF);
	powerDa12_15(numChanel);
//	//скомутировать ключ и включить ключ
	switchOn(numChanel);
	//измеритьx
	float curU = getU_Ad7792(numChanel, codeN);
//	//выключить ключ
	switchOn(100);
	//выключить 1,67 В
	powerDa17_16(P_OFF);
	//выключить 500 мВ
	gnd500mVOff();
	ledRedOff();

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
		case P_1_67_V_REF:
			GPIO_SetBits(GPIOD, GPIO_Pin_5 | GPIO_Pin_2);
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
		default:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			break;
	}
}

void powerDa12_15(uint8_t numChanel)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_10);
	GPIO_ResetBits(GPIOE, GPIO_Pin_1 | GPIO_Pin_0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	switch(numChanel)
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
ResultMes readSenser(uint8_t numChanel, uint16_t *codeN)
{
	static ResultMes result;
	result.uClear = 0;
	uint16_t tempCode;
	if( codeN == 0 )
		codeN = &tempCode;
	switch(configTerem.sensorType[numChanel])
	{
		//Датчики перемещения
		case GT_MM10:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(*codeN, configTerem.a[numChanel][0], 10);
			break;
		case GT_MM20:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(*codeN, configTerem.a[numChanel][0], 20);
			break;
		case GT_MM50:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(*codeN, configTerem.a[numChanel][0], 50);
			break;
		case GT_MM100:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(*codeN, configTerem.a[numChanel][0], 100);
			break;
		case GT_MM200:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(*codeN, configTerem.a[numChanel][0], 200);
			break;
		case GT_HEL700:			//Платиновый ТСП -> в градусах
		case GT_HEL_85:
			powerDa17_16(P_ADC_REF);
			powerDa12_15(numChanel);
			result.uClear = getU_Ad7792(numChanel, codeN);
			result.u = result.uClear * configTerem.a[numChanel][0]
					+ configTerem.a[numChanel][1];
			powerDa17_16(P_OFF);
			powerDa12_15(100);
			if( configTerem.sensorType[numChanel] == GT_HEL700 )
				result.p = HEL700_Termo(result.u, numChanel);
			else
				result.p = HEL700_Termo_85(result.u);
			break;
		case GT_TermoHK:			//Термопара ХК -> в градусах
		case GT_TermoHKcom:
			result.uClear = getU_Ad7792(numChanel, codeN);
			result.u = result.uClear * configTerem.a[numChanel][0]
					+ configTerem.a[numChanel][1];
			result.p = HK_Termo(result.u, tempOfDs1820);
			break;
		case GT_TermoHA:			//Термопара ХА -> в градусах
		case GT_TermoHAcom:
			result.uClear = getU_Ad7792(numChanel, codeN);
			result.u = result.uClear * configTerem.a[numChanel][0]
					+ configTerem.a[numChanel][1];
			result.p = HA_Termo(result.u, tempOfDs1820);
			break;
		case GT_Termo48:			//D Универсальный термопарный вход
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = T48_Termo(result.u, tempOfDs1820, numChanel);
			break;
		case GT_HeatFlowPeltje:
			result.u = getU_Ad7792(numChanel, codeN);
			//result.p = HF_Flow(result.u, koeffsAB.koef[numChanel].a[0]);
			result.p = HF_Flow(result.u, configTerem.a[numChanel][0]);
			break;
		case GT_HeatFlowPeltje48:
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = HF_Flow48(result.u, numChanel);
			break;
		case GT_Relocate:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = Relocate_Length(result.u, koeffsAB.koef[numChanel].a);
			break;
		case GT_HIH3610: //DA17 на 3.0 В
			powerDa17_16(P_3_0);
			powerDa12_15(numChanel);
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = HIH3610_Rh(result.u, tempOfDs1820,
					koeffsAB.koef[numChanel].a);
			powerDa12_15(100);
			powerDa17_16(P_OFF);
			break;
			//Датчики тензометрические
//		case GT_TensoKg:
//				GT_TensoT,   //31, 32 С выводом кг, т
//				GT_TensoN,
//				GT_TensoKN,  //33, 34 С выводом Н, кН
//				GT_TensoKPa,
//				GT_TensoMPa, //35, 36 С выводом кПа, МПа
//				GT_Tenso_uE,
//			break;
			//Инклинометры
//				GT_InclinIN_D3,           //39, спецадаптер инклинометра
//				GT_InclinMK_X,            //3A, спецадаптер с микроконтроллером ATmega
//				GT_InclinMK_Y,            //3B, спецадаптер с микроконтроллером ATmega

//		case GT_SHT1_H_0: //40..47 SHT-10 (влажность) для разных входов microLAN
//				GT_SHT1_H_1,
//				GT_SHT1_H_2,
//				GT_SHT1_H_3,
//				GT_SHT1_H_4,
//				GT_SHT1_H_5,
//				GT_SHT1_H_6,
//				GT_SHT1_H_7,
//				GT_SHT1_T,                //48 SHT-10 (температура)
//				GT_SHT1_DP,               //49 SHT-10 (точка росы)
			break;
		case GT_SHT21:
			dpg2_readValue(numChanel, &result);
			break;

		case GT_R:                     //56 //Сопротивление, кОм
			break;

			//Датчики на основе тензомостов
		case GT_TensoKg2:
		case GT_TensoT2:  //78, 79 С выводом кг, т
		case GT_TensoN2:
		case GT_TensoKN2: //7A, 7B С выводом Н, кН
		case GT_TensoKPa2:
		case GT_TensoMPa2: //7C, 7D С выводом кПа, МПа
		case GT_Tenso_uE2:             //7E, Тензо, относительное удлинение
			powerDa17_16(P_3_0);
			powerDa12_15(numChanel);
			result.u = getU_Ad7792(numChanel, codeN);
			powerDa12_15(100);
			powerDa17_16(P_OFF);
			result.p = Tenso_3(result.u, numChanel, tempOfDs1820);
			break;

			//Напряжение, мВ
		case GT_U:
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = result.u * 1000.0;
			break;
		case GT_U2V:
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = koeffsAB.koef[numChanel].a[0]
					+ result.u * koeffsAB.koef[numChanel].a[1];
			break;
		default:
			result.u = NAN;
			result.p = NAN;
			break;
	}
	return result;
}
