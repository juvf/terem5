/*
 * structCommon.h
 *
 *  Created on: 03 авг. 2015 г.
 *      Author: juvf
 */
#ifndef STRUCTCOMMON_H_
#define STRUCTCOMMON_H_

#include <stdint.h>
#include "stm32f4xx_rtc.h"

//Возможные типы датчиков (изменять одновременно в StructCommon.h для Терем-4.1, адаптера123 и в GaugeModel.h программы связи)
enum
{
	//Датчики температуры
	GT_DS1820_0 = 0,            //DS1820 на выводе 0 (определяется в IO_Ports.h)
	GT_DS1820_1,              //DS1820 на выводе 1
	GT_DS1820_2,              //DS1820 на выводе 2
	GT_DS1820_3,              //DS1820 на выводе 3
	GT_HEL700,                //4 Платиновый ТСП 1000 Ом от Honeywell
	GT_R1,
	GT_R2,
	GT_R3,
	GT_R4,
	GT_TermoHK,   //9 Термопара ХК, дифференциальное подключение (каналы X, X+1)
	GT_TermoHKcom, //A Термопара ХК, псевдодифференциальное подключение (канал X, AinCom)
	GT_TermoHA,               //B Термопара ХА, дифференциальное подключение
	GT_TermoHAcom,          //C Термопара ХА, псевдодифференциальное подключение
	GT_Termo48,               //D Универсальный термопарный вход
	GT_R5,
	GT_R6,
	//Датчики теплового потока
	GT_HeatFlowPeltje,        //10 Датчик теплового потока - элемент Пельтье
	GT_HeatFlowPeltje48, //11 Датчик теплового потока - элемент Пельтье, для 48-канального адаптера
	GT_R7,
	//Датчики влажности
	GT_HIH3610,               //13 HIH3610 от Honeywell
	GT_R8,
	GT_R9,
	GT_R10,
	GT_R11,
	GT_R12,
	GT_R13,
	GT_R14,
	GT_R15,
	GT_R16,
	//Датчики давления
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
	//Датчики перемещения
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
	//Датчики тензометрические
	GT_TensoKg,
	GT_TensoT,   //31, 32 С выводом кг, т
	GT_TensoN,
	GT_TensoKN,  //33, 34 С выводом Н, кН
	GT_TensoKPa,
	GT_TensoMPa, //35, 36 С выводом кПа, МПа
	GT_Tenso_uE,              //37, Тензо, относительное удлинение
	GT_R32,
	//Инклинометры
	GT_InclinIN_D3,           //39, спецадаптер инклинометра
	GT_InclinMK_X,            //3A, спецадаптер с микроконтроллером ATmega
	GT_InclinMK_Y,            //3B, спецадаптер с микроконтроллером ATmega
	GT_R35,
	GT_R36,
	GT_R37,
	GT_R38,
	//SHT-10 (температура, влажность, точка росы)
	GT_SHT1_H_0,          //40..47 SHT-10 (влажность) для разных входов microLAN
	GT_SHT1_H_1,
	GT_SHT1_H_2,
	GT_SHT1_H_3,
	GT_SHT1_H_4,
	GT_SHT1_H_5,
	GT_SHT1_H_6,
	GT_SHT1_H_7,
	GT_SHT1_T,                //48 SHT-10 (температура)
	GT_SHT1_DP,               //49 SHT-10 (точка росы)
	GT_R4A,
	GT_R4B,
	GT_R4C,
	GT_R4D,
	GT_R4E,
	GT_R4F,
	//Датчики температуры
	GT_DS1820_4 = 0x50,      //50 DS1820 на выводе 4 (определяется в IO_Ports.h)
	GT_DS1820_5,              //51 DS1820 на выводе 5
	GT_DS1820_6,              //52 DS1820 на выводе 6
	GT_DS1820_7,              //53 DS1820 на выводе 7
	//Напряжение, мВ
	GT_U,                     //54
	GT_U2V,                   //55
	//Сопротивление, кОм
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
	//Частотные датчики
	GT_F0,
	GT_F1,
	GT_F2,      //60, 61, 62 - Частотные датчики на выводе 0, 1, 2
	GT_F3,
	GT_F4,
	GT_F5,      //63, 64, 65 - на выводе 3, 4, 5
	GT_F6,
	GT_F7,             //66, 67     - на выводе 6, 7
	GT_VIMS,                  //68 - Частотный датчик (цифровой ВИМС)
	GT_R50,
	GT_R51,
	GT_R52,
	GT_R53,
	GT_R54,
	GT_R55,
	GT_R56,
	//Датчики температуры DS1820 + ATtiny
	GT_1820MK_0,
	GT_1820MK_1, //70..77 - DS1820 с микроконтроллером (из РТМ-5)
	GT_1820MK_2,
	GT_1820MK_3,
	GT_1820MK_4,
	GT_1820MK_5,
	GT_1820MK_6,
	GT_1820MK_7,
	//Датчики на основе тензомостов
	GT_TensoKg2,
	GT_TensoT2,  //78, 79 С выводом кг, т
	GT_TensoN2,
	GT_TensoKN2, //7A, 7B С выводом Н, кН
	GT_TensoKPa2,
	GT_TensoMPa2, //7C, 7D С выводом кПа, МПа
	GT_Tenso_uE2,             //7E, Тензо, относительное удлинение

	GT_Absent,                //..-FF Датчик отсутствует, если >= GT_Absent
	GT_AbsentMax = 0xFF
};

//Адреса параметров адаптера
typedef struct
{
	uint8_t sensorType[16];     //0x1000, тип датчика в канале

	unsigned char DF_CompChan[8]; //Номер канала компенсатора для датчика канала 0 (термопары, датчика влажности)

	float Vref;             //Напряжение источника опорного напряжения, В

	uint16_t Flags;             //Различные флаги:
								//Бит 0 - имеется коммутатор аналоговых каналов
								//Бит 1 - имеется коммутатор каналов DS1820
								//Бит 2 - полярность управления выходов номера адаптера
	uint16_t DF_AdapterNum;     //Номер адаптера
	float a[8][2];       //Коэффициенты калибровки датчиков

	uint8_t adcRange[8];

	uint32_t DF_CRC16;          //Контроль целостности
} TeremConfig;

//заголовок процесса
typedef struct
{
	uint32_t count;          //колво точек
	RTC_TimeTypeDef startTime;          //время старта
	RTC_DateTypeDef startDate;          //дата старта
	uint16_t period;          //интервал в сек
	TeremConfig config;          //конфигурация прибора во время процесса
} HeaderProcess;

#endif /* STRUCTCOMMON_H_ */
