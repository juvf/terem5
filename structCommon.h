/*
 * structCommon.h
 *
 *  Created on: 03 ���. 2015 �.
 *      Author: juvf
 */
#ifndef STRUCTCOMMON_H_
#define STRUCTCOMMON_H_

#include <stdint.h>
#include "stm32f4xx_rtc.h"

//��������� ���� �������� (�������� ������������ � StructCommon.h ��� �����-4.1, ��������123 � � GaugeModel.h ��������� �����)
enum
{
	//������� �����������
	GT_DS1820_0 = 0,            //DS1820 �� ������ 0 (������������ � IO_Ports.h)
	GT_DS1820_1,              //DS1820 �� ������ 1
	GT_DS1820_2,              //DS1820 �� ������ 2
	GT_DS1820_3,              //DS1820 �� ������ 3
	GT_HEL700,                //4 ���������� ��� 1000 �� �� Honeywell
	GT_R1,
	GT_R2,
	GT_R3,
	GT_R4,
	GT_TermoHK,   //9 ��������� ��, ���������������� ����������� (������ X, X+1)
	GT_TermoHKcom, //A ��������� ��, ���������������������� ����������� (����� X, AinCom)
	GT_TermoHA,               //B ��������� ��, ���������������� �����������
	GT_TermoHAcom,          //C ��������� ��, ���������������������� �����������
	GT_Termo48,               //D ������������� ����������� ����
	GT_R5,
	GT_R6,
	//������� ��������� ������
	GT_HeatFlowPeltje,        //10 ������ ��������� ������ - ������� �������
	GT_HeatFlowPeltje48, //11 ������ ��������� ������ - ������� �������, ��� 48-���������� ��������
	GT_R7,
	//������� ���������
	GT_HIH3610,               //13 HIH3610 �� Honeywell
	GT_R8,
	GT_R9,
	GT_R10,
	GT_R11,
	GT_R12,
	GT_R13,
	GT_R14,
	GT_R15,
	GT_R16,
	//������� ��������
	GT_MPX5700,               //1D
	GT_R17,
	GT_R18,
	GT_R19,
	GT_R20,
	GT_R21,
	GT_R22,
	GT_R23,
	GT_R24,
	GT_R25,
	//������� �����������
	GT_MM10,
	GT_MM20,
	GT_MM50, //27, 28, 29
	GT_Relocate,               //30
	GT_Rel_Ind,                //31
	GT_R26,
	GT_R27,
	GT_R28,
	GT_R29,
	GT_R30,
	//������� ����������������
	GT_TensoKg,
	GT_TensoT,   //31, 32 � ������� ��, �
	GT_TensoN,
	GT_TensoKN,  //33, 34 � ������� �, ��
	GT_TensoKPa,
	GT_TensoMPa, //35, 36 � ������� ���, ���
	GT_Tenso_uE,              //37, �����, ������������� ���������
	GT_R32,
	//������������
	GT_InclinIN_D3,           //39, ����������� ������������
	GT_InclinMK_X,            //3A, ����������� � ����������������� ATmega
	GT_InclinMK_Y,            //3B, ����������� � ����������������� ATmega
	GT_R35,
	GT_R36,
	GT_R37,
	GT_R38,
	//SHT-10 (�����������, ���������, ����� ����)
	GT_SHT1_H_0,          //40..47 SHT-10 (���������) ��� ������ ������ microLAN
	GT_SHT1_H_1,
	GT_SHT1_H_2,
	GT_SHT1_H_3,
	GT_SHT1_H_4,
	GT_SHT1_H_5,
	GT_SHT1_H_6,
	GT_SHT1_H_7,
	GT_SHT1_T,                //48 SHT-10 (�����������)
	GT_SHT1_DP,               //49 SHT-10 (����� ����)
	GT_R4A,
	GT_R4B,
	GT_R4C,
	GT_R4D,
	GT_R4E,
	GT_R4F,
	//������� �����������
	GT_DS1820_4 = 0x50,      //50 DS1820 �� ������ 4 (������������ � IO_Ports.h)
	GT_DS1820_5,              //51 DS1820 �� ������ 5
	GT_DS1820_6,              //52 DS1820 �� ������ 6
	GT_DS1820_7,              //53 DS1820 �� ������ 7
	//����������, ��
	GT_U,                     //54
	GT_U2V,                   //55
	//�������������, ���
	GT_R,                     //56
	GT_R41,
	GT_R42,
	GT_R43,
	GT_R44,
	GT_R45,
	GT_R46,
	GT_R47,
	GT_R48,
	GT_R49,
	//��������� �������
	GT_F0,
	GT_F1,
	GT_F2,      //60, 61, 62 - ��������� ������� �� ������ 0, 1, 2
	GT_F3,
	GT_F4,
	GT_F5,      //63, 64, 65 - �� ������ 3, 4, 5
	GT_F6,
	GT_F7,             //66, 67     - �� ������ 6, 7
	GT_VIMS,                  //68 - ��������� ������ (�������� ����)
	GT_R50,
	GT_R51,
	GT_R52,
	GT_R53,
	GT_R54,
	GT_R55,
	GT_R56,
	//������� ����������� DS1820 + ATtiny
	GT_1820MK_0,
	GT_1820MK_1, //70..77 - DS1820 � ����������������� (�� ���-5)
	GT_1820MK_2,
	GT_1820MK_3,
	GT_1820MK_4,
	GT_1820MK_5,
	GT_1820MK_6,
	GT_1820MK_7,
	//������� �� ������ �����������
	GT_TensoKg2,
	GT_TensoT2,  //78, 79 � ������� ��, �
	GT_TensoN2,
	GT_TensoKN2, //7A, 7B � ������� �, ��
	GT_TensoKPa2,
	GT_TensoMPa2, //7C, 7D � ������� ���, ���
	GT_Tenso_uE2,             //7E, �����, ������������� ���������

	GT_Absent,                //..-FF ������ �����������, ���� >= GT_Absent
	GT_AbsentMax = 0xFF
};

//������ ���������� ��������
typedef struct
{
	uint8_t sensorType[16];     //0x1000, ��� ������� � ������

	unsigned char DF_CompChan[8]; //����� ������ ������������ ��� ������� ������ 0 (���������, ������� ���������)

	float Vref;             //���������� ��������� �������� ����������, �

	uint16_t Flags;             //��������� �����:
								//��� 0 - ������� ���������� ���������� �������
								//��� 1 - ������� ���������� ������� DS1820
								//��� 2 - ���������� ���������� ������� ������ ��������
	uint16_t DF_AdapterNum;     //����� ��������
	float a[8][2];       //������������ ���������� ��������

	uint8_t adcRange[8];

	uint16_t crc[2];          //�������� �����������
} TeremConfig;

//��������� ��������
typedef struct
{
	uint32_t count;          //����� �����
	RTC_TimeTypeDef startTime;          //����� ������
	RTC_DateTypeDef startDate;          //���� ������
	uint32_t period;          //�������� � ���
	TeremConfig config;          //������������ ������� �� ����� ��������
} HeaderProcess;

#endif /* STRUCTCOMMON_H_ */
