/*
 * Sensor.cpp
 *
 *  Created on: 06 ���. 2015 �.
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
#include <math.h>

//����������� ��������� ����
//#define  TEMP_HS	22

//����� �� 0 �� 7
float readAnalogSensor(uint8_t numChanel, uint16_t *codeN)
{
	if( numChanel > 7 )
		return -1;
	//������ +500 �� �� 1 ����
	gnd500mVOn(); //������ +500 �� �� 1 ����
//	gnd500mVOff();
	//������ 1,67 � �� 4 ����
	powerDa17_16(P_1_67);
	powerDa12_15(numChanel);
//	//������������� ���� � �������� ����
	switchOn(numChanel);
	//��������x
	float curU = getU_Ad7792(numChanel, codeN);
//	//��������� ����
	switchOn(100);
	//��������� 1,67 �
	powerDa17_16(P_OFF);
	//��������� 500 ��
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

//������� ��������� ������� ---------------------------------------------------
//����: ��������, ��������� �� ������������ (������� � A4)
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

//������� ������� ������� -----------------------------------------------------
//����: ��������, ��������� �� ������������ (������� � A3)
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
	switch(configTerem.sensorType[numChanel])
	{
		//������� �����������
		case GT_MM10:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(result.u, configTerem.a[numChanel][0], 11);
			//				valP = valU * 2.0 / 1.17;
			//				valP = (valP - configTerem.a[buffer[6]][0]) * 5.5; //��������� � ��
			break;
		case GT_MM20:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(result.u, configTerem.a[numChanel][0], 20);
			break;
		case GT_MM50:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(result.u, configTerem.a[numChanel][0], 50);
			break;
		case GT_MM100:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(result.u, configTerem.a[numChanel][0], 100);
			break;
		case GT_MM200:
			result.u = readAnalogSensor(numChanel, codeN);
			result.p = MMxx_Length(result.u, configTerem.a[numChanel][0], 200);
			break;
		case GT_HEL700:			//���������� ��� -> � ��������
		case GT_HEL_85:
			powerDa17_16(P_ADC_REF);
			powerDa12_15(numChanel);
			result.u = getU_Ad7792(numChanel, codeN);
			powerDa17_16(P_OFF);
			powerDa12_15(100);
			if( configTerem.sensorType[numChanel] == GT_HEL700 )
				result.p = HEL700_Termo(result.u, numChanel);
			else
				result.p = HEL700_Termo_85(result.u);
			break;
		case GT_TermoHK:			//��������� �� -> � ��������
		case GT_TermoHKcom:
			result.uClear = getU_Ad7792(numChanel, codeN);
			result.u = result.uClear * configTerem.a[numChanel][0]
					+ configTerem.a[numChanel][1];
			result.p = HK_Termo(result.u, tempOfDs1820);
			break;
		case GT_TermoHA:			//��������� �� -> � ��������
		case GT_TermoHAcom:
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = HA_Termo(result.u, tempOfDs1820);
			break;
		case GT_Termo48:			//D ������������� ����������� ����
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
		case GT_HIH3610: //DA17 �� 3.0 �
			powerDa17_16(P_3_0);
			powerDa12_15(numChanel);
			result.u = getU_Ad7792(numChanel, codeN);
			result.p = HIH3610_Rh(result.u, tempOfDs1820,
					koeffsAB.koef[numChanel].a);
			powerDa12_15(100);
			powerDa17_16(P_OFF);
			break;
			//������� ����������������
//		case GT_TensoKg:
//				GT_TensoT,   //31, 32 � ������� ��, �
//				GT_TensoN,
//				GT_TensoKN,  //33, 34 � ������� �, ��
//				GT_TensoKPa,
//				GT_TensoMPa, //35, 36 � ������� ���, ���
//				GT_Tenso_uE,
//			break;
			//������������
//				GT_InclinIN_D3,           //39, ����������� ������������
//				GT_InclinMK_X,            //3A, ����������� � ����������������� ATmega
//				GT_InclinMK_Y,            //3B, ����������� � ����������������� ATmega

//		case GT_SHT1_H_0: //40..47 SHT-10 (���������) ��� ������ ������ microLAN
//				GT_SHT1_H_1,
//				GT_SHT1_H_2,
//				GT_SHT1_H_3,
//				GT_SHT1_H_4,
//				GT_SHT1_H_5,
//				GT_SHT1_H_6,
//				GT_SHT1_H_7,
//				GT_SHT1_T,                //48 SHT-10 (�����������)
//				GT_SHT1_DP,               //49 SHT-10 (����� ����)
			break;
		case GT_SHT21:
			dpg2_readValue(numChanel, &result);
			break;

		case GT_R:                     //56 //�������������, ���
			break;

			//������� �� ������ �����������
		case GT_TensoKg2:
		case GT_TensoT2:  //78, 79 � ������� ��, �
		case GT_TensoN2:
		case GT_TensoKN2: //7A, 7B � ������� �, ��
		case GT_TensoKPa2:
		case GT_TensoMPa2: //7C, 7D � ������� ���, ���
		case GT_Tenso_uE2:             //7E, �����, ������������� ���������
			powerDa17_16(P_3_0);
			powerDa12_15(numChanel);
			result.u = getU_Ad7792(numChanel, codeN);
			powerDa12_15(100);
			powerDa17_16(P_OFF);
			result.p = Tenso_3(result.u, numChanel, tempOfDs1820);
			break;

			//����������, ��
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
