/*
 * ds1820.cpp
 *
 *  Created on: 13 дек. 2015 г.
 *      Author: anisimov.e
 */
#include "ds1820.h"
#include "osConfig.h"
#include "stm32f4xx.h"

//void initDS1820()
//{
//	//Выдача команды преобразования
//	uLAN_SkipROM();
//	uLAN_WrBYTE(0x44);
//}
//
////Обращение ко всем датчикам ----------------------------------------------2 мс
////Выход: err1820=0 - OK, 1 - нет ответа от датчиков (1 на линии)
////       2 - затянут ответ после Reset, 3 - не прошла запись команды
//uint8_t uLAN_SkipROM()
//{
//	uLAN_Reset();       //Сброс датчиков и прием Presence Pulse
//	uLAN_WrBYTE(0xCC);  //Выдача команды
//	return 0;
//}
//
//void uLAN_WrBYTE(uint8_t byte)
//{
//
//}
//
////Сброс и прием Presence Pulse ------------------------------------------------
//uint8_t uLAN_Reset()
//{
//	uint8_t err_uLAN = 0;             //Сбросить код ошибки самоконтроля
//	uLAN0();             //Обнуление сигнальной линии и переключение ее на выход
//	vTaskDelay(550);//mksDelayCCR(550);               //480...960 мкс => 500 (720) mkS
//	_DINT();
//	uLANZ();                        //Восстановление сигнальной линии (Z)
//	vTaskDelay(70);//mksDelayCCR(70);                //Ожидание ответа от датчика (-ов)
//	if( uLANis_1() )
//		err_uLAN = 1;     //Нет ответа
//	mksDelayCCR(250);
//	if( !uLANis_1() )
//		err_uLAN = 2;     //Ответ затянулся
//	return err_uLANl;
//}

//Вот мой рабочий код. Подключение 3-х проводное,
//работа с одним датчиком без CRC. Использование простое temp=readtemp();
//Ответ 273=27.3, отрицательные не работают, но для проверки годится.

//установки для вывода термодатчика
#define DQ_IN()  (GPIOB->MODER &= ~0x300000) //  Input mode (reset state)
#define DQ_OUT() (GPIOB->MODER |= 0x100000) //  General purpose output mode.
#define S_DQ()   (GPIOB->BSRRL = GPIO_Pin_10) //set pin
#define C_DQ()   (GPIOB->BSRRH  = GPIO_Pin_10) //reset pin
#define DQ()     (GPIOB->IDR & GPIO_Pin_10) //check pin

//----------------------------------------------------------------
// Start transaction with 1-wire line.
void init_ds18b20()
{
	DQ_OUT();
	C_DQ();
	mksDelay(550);
	S_DQ();
	DQ_IN();
	mksDelay(50);
	while(DQ() != 0)
		; //wait for DQ low
	mksDelay(240);
	DQ_OUT();
	S_DQ();
	mksDelay(300);
}

void mksDelay(uint16_t time)
{
	while(--time)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
//----------------------------------------------------------------
// Read a byte from the sensor
uint8_t readbyte()
{
	uint8_t i = 0, data = 0;
	DQ_OUT();
	for(i = 0; i < 8; i++)
	{
		C_DQ();
		data >>= 1;
		mksDelay(3);
		S_DQ();
		DQ_IN();
		mksDelay(12);
		if( DQ() )
			data |= 0x80;
		DQ_OUT();
		S_DQ();
		mksDelay(45);
		mksDelay(5);
	}
	return (data);
}
//----------------------------------------------------------------
// Write a command to the sensor
void writecommand(uint8_t data)
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		C_DQ();
		mksDelay(15);
		if( data & 0x01 )
			S_DQ();
		else
			C_DQ();
		mksDelay(45);
		data >>= 1;
		S_DQ();
		mksDelay(2);
	}
}

//----------------------------------------------------------------
// Read value from the sensor
float readtemp()
{
	uint16_t L, H, ds;

	init_ds18b20();
	// Convert
	writecommand(0xCC);
	writecommand(0x44);
	init_ds18b20();
//   // Read Scratch memory area
	writecommand(0xCC);
	writecommand(0xBE);
	L = readbyte();
	H = readbyte();
	ds = (H << 8) + L;
//resolution 0.0625

	return (float)ds * 0.0625;
}
