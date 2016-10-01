/*
 * adc.h
 *
 *  Created on: 18 авг. 2015 г.
 *      Author: juvf
 */

#ifndef ADC_H_
#define ADC_H_
#include "stm32f4xx.h"
#include <stdint.h>

#ifdef __cplusplus


/************************************************************
* STANDARD BITS
************************************************************/

#define BIT0                (0x0001u)
#define BIT1                (0x0002u)
#define BIT2                (0x0004u)
#define BIT3                (0x0008u)
#define BIT4                (0x0010u)
#define BIT5                (0x0020u)
#define BIT6                (0x0040u)
#define BIT7                (0x0080u)
#define BIT8                (0x0100u)
#define BIT9                (0x0200u)
#define BITA                (0x0400u)
#define BITB                (0x0800u)
#define BITC                (0x1000u)
#define BITD                (0x2000u)
#define BITE                (0x4000u)
#define BITF                (0x8000u)


//#define                 //Коммуникационный регистр
#define   CR7_WEN   BIT7  //Разрешение записи (0)
#define   CR6_RW    BIT6  //Чтение (1) / запись (0)
#define   CR3_RS_3  BIT3  //Адрес регистра (3 бита)
                            //0-коммуникационный/статуса, 8 бит
                            //1-Mode, 16 бит
                            //2-Configuration, 16 бит
                            //3-Data, 16 бит
                            //4-ID, 8 бит
                            //5-IO, 8 бит
                            //6-Offset, 16 бит
                            //7-Full scale, 16 бит
#define   CR2_CREAD BIT2  //Режим продолжительного чтения (1)

//Регистр статуса (байт)
#define SR          0
#define SR7_RDY     BIT7    //Готовность (0) данных
#define SR6_ERR     BIT6    //Переполнение (1) данных (все 0 или все 1)
#define SR3_7792    BIT3    //Для AD7792 всегда 0
#define SR0_CH_3    BIT0    //Канал (3 бита)

//Регистр режима работы (слово)
#define MODE        1
#define MR13_MD_3   BITD    //Выбор режима (3 бита)
                              //0-непрерывное преобразование
                              //1-однократное преобразование
                              //2-Idle
                              //3-Power Down
                              //4-внутренняя калибровка нуля
                              //5-внутренняя калибровка полной шкалы
                              //6-системная калибровка нуля
                              //7-системная калибровка полной шкалы
#define MR6_CLK_2   BIT6    //Выбор тактирования (2 бита)
                              //0-внутренний 64 кГц
                              //1-то же с выводом наружу
                              //2-внешние 64 кГц
                              //3-внешнее тактирование с делением /2
#define MR0_FS_4    BIT0    //Частота обновления фильтра (4 бита)
                              //0-не используется
                              //1-500 Гц, 4 мс
                              //2-250 Гц, 8 мс
                              //3-125 Гц, 16 мс
                              //4-62,5 Гц, 32 мс
                              //5-50 Гц, 40 мс
                              //6-39,2 Гц, 48 мс
                              //7-33,3 Гц, 60 мс
                              //8-19,6 Гц, 101 мс (подавление 60 Гц 90 дБ)
                              //9-16,7 Гц, 120 мс (подавление 50 Гц 80 дБ)
                              //10-16,7 Гц, 120 мс (подавление 50-60 Гц 65 дБ)
                              //11-12,5 Гц, 160 мс (подавление 50-60 Гц 66 дБ)
                              //12-10 Гц, 200 мс (подавление 50-60 Гц 69 дБ)
                              //13-8,33 Гц, 240 мс (подавление 50-60 Гц 70 дБ)
                              //14-6,25 Гц, 320 мс (подавление 50-60 Гц 72 дБ)
                              //15-4,17 Гц, 480 мс (подавление 50-60 Гц 74 дБ)
//Регистр конфигурации (слово)
#define CON         2
#define CON14_VBIAS_2 BITE  //Разрешение напряжения смещения
                              //0-запрещен
                              //1-подключено к AIN1(-)
                              //2-подключено к AIN2(-)
                              //3-резерв
#define CON13_B0    BITD  //Разрешение 100 нА источника тока (1)
#define CON12_UB    BITC  //однополярный (1) / биполярный (0) режим
#define CON11_BOOST BITB  //Умощнение источника напряжения смещения (1)
#define CON8_G_3    BIT8  //Усиление
                            //0-1 (шкала 2,5 В или 1,17 В)
                            //1-2 (1,25 В или 585 мВ)
                            //2-4 (625 мВ или 292,5 мВ)
                            //3-8 (312,5 мВ или 146,25 мВ)
                            //4-16 (156,2 мВ или 73,125 мВ)
                            //5-32 (78,125 мВ или 36,5625 мВ)
                            //6-64 (39,06 мВ или 18,28125 мВ)
                            //7-128 (19,53 мВ или 9,140625 мВ)
#define CON7_REFSEL BIT7  //Внешнее (0) / внутреннее (1) опорное напряжение
#define CON4_BUF    BIT4  //Буферирование (1) входного сигнала
#define CON0_CH_3   BIT0  //Выбор канала измерения (3 бита)
                            //0-AIN1(+)-AIN1(-)
                            //1-AIN2(+)-AIN2(-)
                            //2-AIN3(+)-AIN3(-)
                            //3-AIN1(-)-AIN1(-)
                            //6-датчик температуры
                            //7-монитор AVdd
//Регистр данных (слово)
#define DATA        3

//Регистр идентификации (байт)
#define ID          4

//Регистр разрешения источника тока (байт)
#define IO          5
#define IO2_IEXCDIR_2 BIT2  //Направление источника тока
                              //0-IEXC1 подключен к IOUT1, IEXC2 - к IOUT2
                              //1-IEXC1 подключен к IOUT2, IEXC2 - к IOUT1
                              //2-оба источника подключены к IOUT1 (для 10 мкА или 210 мкА)
                              //3-оба источника подключены к IOUT2 (для 10 мкА или 210 мкА)
#define IO0_IEXCEN_2 BIT0   //Разрешение источника тока
                              //0-запрещен
                              //1-10 мкА
                              //2-210 мкА
                              //3-1 мА

//Регистр смещения (для калибровки нуля) (слово)
#define OFFSET      6

//Регистр полной шкалы (для калибровки полной шкалы) (слово)
#define FULL        7

void initSpi1();
uint8_t initAdc();
uint8_t AD7792Rd(uint8_t reg);
void AD7792WrW(uint8_t reg, uint16_t data);
uint16_t AD7792RdW(unsigned char Register);
void AD7792Wr(unsigned char Register, unsigned char Data);
uint16_t AD7792Measure();
//float getU_Ad7792(unsigned char *curRange, unsigned char type);
float getU_Ad7792(unsigned char numChanel, uint16_t *code = 0);
void IO_420();
void AD7792Calibr();
void AD7792Calibr7();
void IO_Off();
float GainKoef(unsigned char Range);


//--- функции по работе с внетренним АЦП
void initIntAdc();
uint16_t getBatValue();

extern "C"
{
#endif
void spiPortAdcOn();
void spiPortAdcOff();

#ifdef __cplusplus
}
#endif


#endif /* ADC_H_ */
