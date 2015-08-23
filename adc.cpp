/*
 * adc.cpp
 *
 *  Created on: 18 авг. 2015 г.
 *      Author: juvf
 */
#include "adc.h"
#include "structCommon.h"
#include "tasks/configTerem.h"
#include "osConfig.h"

#define csOn()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define csOff()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

//Определение функции ожидания готовности (нуля на MISO)
#define IsMISOSet() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) != 0)
#define ReadyWait()  while(IsMISOSet())

//Частота обновления фильтра 11: 12.5 Гц, 160 мс (подавление 50-60 Гц 66 дБ)
//                            9: 16,7 Гц, 120 мс (подавление 50 Гц 80 дБ)
//                            1: 500 Гц, 4 мс
#define FS  9

unsigned char CurRangeADC = 0;
uint16_t CalFull_1;  //Значения калибровочных коэффициентов

void initSpi1()
{
	GPIO_InitTypeDef gpio;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &gpio);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	gpio.GPIO_Pin = GPIO_Pin_4;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &gpio);
	csOff();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitTypeDef spiInit;
	spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spiInit.SPI_CPHA = SPI_CPHA_2Edge;
	spiInit.SPI_CPOL = SPI_CPOL_High;
	spiInit.SPI_CRCPolynomial = 7;
	spiInit.SPI_DataSize = SPI_DataSize_8b;
	spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInit.SPI_Mode = SPI_Mode_Master;
	spiInit.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI1, &spiInit);
	SPI_Cmd(SPI1, ENABLE);
}

//Инициализация ---------------------------------------------------------------
//Выход: 0-ОК или код ошибки при отсутствии AD7792
uint8_t initAdc()
{
	csOn();
	//Сброс "после потери синхронизации" - 32 бита 1
//	SPI_I2S_SendData(SPI1, 0xff);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//		;
//	SPI_I2S_SendData(SPI1, 0xff);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//		;
//	SPI_I2S_SendData(SPI1, 0xff);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//		;
//	SPI_I2S_SendData(SPI1, 0xff);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
//		;

	//Чтение регистра идентификации (д.б. 0xXA)
	uint8_t regId = AD7792Rd(ID);
	if((regId & 0x0F) != 0x0A)
	{              //Ошибка, не тот ответ
		csOff();
		return regId;
	}

	//Выбор Idle-режима работы
	AD7792WrW(MODE,           //Регистр режима работы
			(MR13_MD_3 * 2) |       //Idle
					(MR6_CLK_2 * 0) |       //Внутреннее тактирование
					(MR0_FS_4 * FS)         //Частота обновления фильтра
					);

	csOff();
	return 0;
}

void AD7792WrW(uint8_t reg, uint16_t data)
{         //reg - Адрес записываемого регистра
	SPI_I2S_SendData(SPI1, reg * CR3_RS_3);  //Запись в коммуникационный регистр
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);
	//Запись данных в нужный регистр
	SPI_I2S_SendData(SPI1, (uint8_t)(data >> 8));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);
	SPI_I2S_SendData(SPI1, (uint8_t)data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);
}

uint8_t AD7792Rd(uint8_t reg)
{
	SPI_I2S_SendData(SPI1, ((CR6_RW * 1) | (reg * CR3_RS_3)));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);
	SPI_I2S_SendData(SPI1, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);
	return SPI_I2S_ReceiveData(SPI1);
}

//Измерение -------------------------------------------------------------------
//Выход: измеренное значение в кодах
//#pragma inline  777 закомитил инлайн
uint16_t AD7792Measure()
{
	AD7792WrW(MODE,           //Регистр режима работы
			(MR13_MD_3 * 1) |       //Однократное преобразование (1)
					(MR6_CLK_2 * 0) |       //Внутреннее тактирование
					(MR0_FS_4 * FS)         //Частота обновления фильтра
					);
	//Ждем готовности, читаем результат в кодах
	ReadyWait()
		;
//	while(PIN_SPI & Bit_SPI_MISO)
//		;
	return AD7792RdW(DATA);
}

uint16_t AD7792RdW(unsigned char Register)
{
	uint8_t data[2];
	SPI_I2S_SendData(SPI1, (CR6_RW * 1) | (Register * CR3_RS_3));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_ReceiveData(SPI1);

	SPI_I2S_SendData(SPI1, 0);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	data[0] = SPI_I2S_ReceiveData(SPI1);

	SPI_I2S_SendData(SPI1, 0);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	data[1] = SPI_I2S_ReceiveData(SPI1);

	return (data[0] << 8) | data[1];
}

//=============================================================================
//Измерение напряжения с автоопределением диапазона ---------------------------
float getU_Ad7792(unsigned char numChanel)
{
	uint8_t *CurRange = &configTerem.adcRange[numChanel];
	uint16_t CurCode;               //Текущее значение в кодах
	float curU;                 //Текущее значение напряжения в вольтах

	//Для HEL700
	if(configTerem.sensorType[numChanel] == GT_HEL700)
	{
		csOn();  //Подача Chip Select
		IO_420();     //Источники тока 2*210 мкА на IOUT2
		//Диапазон измерений (-50..+270)°С соответствует опорному резистору 2 кОм
		AD7792WrW(CON,            //Запись в регистр конфигурации
				(CON14_VBIAS_2 * 0) |   //Напряжение смещения не подавать (0)
						(CON13_B0 * 0) |   //Источник тока 100 нА выключен (0)
						(CON12_UB * 1) |   //Однополярный режим (1)
						(CON11_BOOST * 0) | //Умощнение источника напряжения смещения выключено (0)
						(CON8_G_3 * 0) |   //Усиление 1 (0)
						(CON7_REFSEL * 0) |   //Опорник внешний (0)
						(CON4_BUF * 1) | //Буферирование входного сигнала включено (1)
						(CON0_CH_3 * 0)         //Канал AIN1 (0)
						);
		//Калибровка канала 1 (всегда, т.к. могут быть обрывы и ограничение)
		AD7792Calibr();
		csOff(); //Убрать Chip Select
		CurRangeADC = 0xFF;
		//Если калибровочный регистр в ауте, не продолжать
		//if(CalFull_1 <= 0x100) {U[Ad][Channel]=flErrorCode; continue;}
		//Измерение
		csOn();
		CurCode = AD7792Measure();
		IO_Off(); //Источники тока отключить
		csOff();
		if((CurCode == 0) || (CurCode == 0xFFFF))
			//gFlags.BadResult = 1;
			;
		else //           Код   2кОм 16 бит
		{
			curU = CurCode * (2.0 / 0xFFFF);
			//gFlags.BadResult = 0;
		}
		//Status=AD7792Rd(SR);

		/*
		 //Для тензодатчиков
		 }else if((Type>=GT_TensoKg) && (Type<=GT_TensoMPa)) {
		 IO_Off(); //Источники тока отключить
		 AD7792WrW(CON,            //Запись в регистр конфигурации
		 (CON14_VBIAS_2 * 0) |   //Напряжение смещения не подавать (0)
		 (CON13_B0 * 0)      |   //Источник тока 100 нА выключен (0)
		 (CON12_UB * 0)      |   //Двуполярный режим (0)
		 (CON11_BOOST * 0)   |   //Умощнение источника напряжения смещения выключено (0)
		 (CON8_G_3 * 7)      |   //Усиление 128
		 (CON7_REFSEL * 1)   |   //Опорник внутренний (1)
		 (CON4_BUF * 1)      |   //Буферирование входного сигнала включено (1)
		 (CON0_CH_3 * 0)         //Канал AIN1 (0)
		 );
		 //Калибровка канала 1
		 if(CurRangeADC != 7) {AD7792Calibr(); CurRangeADC=7;}
		 //Измерение
		 CurCode=AD7792Measure();
		 //Текущее значение напряжения в вольтах (двуполярный режим)
		 U[Ad][Channel] = (CurCode*(1.17/128/0x8000)-(1.17/128));
		 */
	}
	else
	{ //Для остальных типов датчиков
		csOn();
		IO_Off(); //Источники тока отключить
		csOff();
		//Цикл измерения с автоопределением диапазона
		while(1)
		{
			csOn();
			AD7792WrW(CON,            //Запись в регистр конфигурации
					(CON14_VBIAS_2 * 0) | //Напряжение смещения не подавать (0)
							(CON13_B0 * 0) | //Источник тока 100 нА выключен (0)
							(CON12_UB * 0) | //Двуполярный режим (0)
							(CON11_BOOST * 0) | //Умощнение источника напряжения смещения выключено (0)
							(CON8_G_3 * *CurRange) | //Усиление
							(CON7_REFSEL * 1) | //Опорник внутренний (1)
							(CON4_BUF * 1) | //Буферирование входного сигнала включено (1)
							(CON0_CH_3 * 0)         //Канал AIN1 (0)
							);
			//Калибровка канала 1
			if(CurRangeADC != *CurRange)
			{
				CurRangeADC = *CurRange;
				if(*CurRange != 7)
					AD7792Calibr();
				else
					AD7792Calibr7();
				csOff();
			}
//			for(;;)
			{
				//Измерение
				csOn();
				CurCode = AD7792Measure();
				csOff();
//				vTaskDelay(1000);
			}
			//Перегрузка (+), уменьшить коэффициент усиления PGA
			if(CurCode == 0xFFFF)
			{
				if(*CurRange)
				{
					if(--(*CurRange))
						(*CurRange)--;
					//gFlags.RangeChanged = 1;
				}
				else if((configTerem.sensorType[numChanel] >= GT_MM10)
						&& (configTerem.sensorType[numChanel] <= GT_Rel_Ind))
				{
					//gFlags.BadResult = 0;
					break;
				}
				else
				{
					//gFlags.BadResult = 1;
					break;
				}
				//Перегрузка (-), уменьшить коэффициент усиления
			}
			else if(!CurCode)
			{
				if(*CurRange)
				{
					if(--(*CurRange))
						(*CurRange)--;
					//gFlags.RangeChanged = 1;
				}
				else
				{
					//gFlags.BadResult = 1;
					break;
				}
				//Недостаточное использование разрядности
			}
			else if((CurCode < 0x8800) && (CurCode > 0x7800) && (*CurRange < 7))
			{
				//Увеличить коэффициент усиления PGA для увеличения точности
				while((CurCode < 0x8800) && (CurCode > 0x7800)
						&& (*CurRange < 7))
				{
					(*CurRange)++;
					//gFlags.RangeChanged = 1;
					CurCode = (((signed)(CurCode - 0x8000)) * 2) + 0x8000;
				}
				//Измерение полноценное
			}
			else
			{
				//gFlags.BadResult = 0;
				break;
			}
		}
		curU = GainKoef(*CurRange) * (int16_t)(CurCode - 0x8000);
	}
	return curU;
}

//Вкл/выкл источников тока ----------------------------------------------------
void IO_420()
{ //2*210=420 мкА
	AD7792Wr(IO,              //Регистр IO
			(IO2_IEXCDIR_2 * 3) |   //Подключение IEXC1, IEXC2 к IOUT2
					(IO0_IEXCEN_2 * 2) //2 * 210 мкА (0.84 В на опоре, до 0.75 на HEL700 - 200°C)
					);
}

//Обращение к регистрам AD7792 ------------------------------------------------
void AD7792Wr(unsigned char Register, unsigned char Data)
{
	//ShiftSPIw(((Register * CR3_RS_3) << 8) + Data);

	SPI_I2S_SendData(SPI1, Register * CR3_RS_3);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, Data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
}

//Калибровка ------------------------------------------------------------------
void AD7792Calibr()
{
	AD7792WrW(MODE,           //Регистр режима работы
			(MR13_MD_3 * 5) |       //Внутренняя калибровка полной шкалы
					(MR6_CLK_2 * 0) |       //Внутреннее тактирование
					(MR0_FS_4 * FS)         //Частота обновления фильтра
					);
	//Ждем готовности, читаем результат
	ReadyWait()
		;
	CalFull_1 = AD7792RdW(FULL);
}

void IO_Off()
{ //Выкл
	AD7792Wr(IO,              //Регистр IO
			(IO2_IEXCDIR_2 * 2) |   //Подключение IEXC1, IEXC2 к IOUT1
					(IO0_IEXCEN_2 * 0)     //запрещены
					);
}

void AD7792Calibr7()
{
	//Временный переход к усилению 6
	AD7792WrW(CON,            //Запись в регистр конфигурации
			(CON14_VBIAS_2 * 0) |   //Напряжение смещения не подавать (0)
					(CON13_B0 * 0) |   //Источник тока 100 нА выключен (0)
					(CON12_UB * 0) |   //Двуполярный режим (0)
					(CON11_BOOST * 0) | //Умощнение источника напряжения смещения выключено (0)
					(CON8_G_3 * 6) |   //Усиление
					(CON7_REFSEL * 1) |   //Опорник внутренний (1)
					(CON4_BUF * 1) | //Буферирование входного сигнала включено (1)
					(CON0_CH_3 * 0)         //Канал AIN1 (0)
					);
	AD7792WrW(MODE,           //Регистр режима работы
			(MR13_MD_3 * 5) |       //Внутренняя калибровка полной шкалы
					(MR6_CLK_2 * 0) |       //Внутреннее тактирование
					(MR0_FS_4 * FS)         //Частота обновления фильтра
					);
	//Ждем готовности, читаем результат
	ReadyWait()
		;
	CalFull_1 = AD7792RdW(FULL);
	//Возврат к усилению 7
	AD7792WrW(CON,            //Запись в регистр конфигурации
			(CON14_VBIAS_2 * 0) |   //Напряжение смещения не подавать (0)
					(CON13_B0 * 0) |   //Источник тока 100 нА выключен (0)
					(CON12_UB * 0) |   //Двуполярный режим (0)
					(CON11_BOOST * 0) | //Умощнение источника напряжения смещения выключено (0)
					(CON8_G_3 * 7) |   //Усиление
					(CON7_REFSEL * 1) |   //Опорник внутренний (1)
					(CON4_BUF * 1) | //Буферирование входного сигнала включено (1)
					(CON0_CH_3 * 0)         //Канал AIN1 (0)
					);
}

//Получение коэф. усиления из кода --------------------------------------------
float GainKoef(unsigned char Range)
{
	switch(Range)
	{
		default:
			return 1.17 / 0x7FFF;
		case 1:
			return 1.17 / 2 / 0x7FFF;
		case 2:
			return 1.17 / 4 / 0x7FFF;
		case 3:
			return 1.17 / 8 / 0x7FFF;
		case 4:
			return 1.17 / 16 / 0x7FFF;
		case 5:
			return 1.17 / 32 / 0x7FFF;
		case 6:
			return 1.17 / 64 / 0x7FFF;
		case 7:
			return 1.17 / 128 / 0x7FFF;
	}
}

