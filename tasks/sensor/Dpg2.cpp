/*
 * Dpg2.cpp
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: anisimov.e
 */

#include "Dpg2.h"
#include "Sensor.h"
#include "stm32f4xx.h"
#include "../CritSect.h"
#include "osConfig.h"

#include <string.h>

extern void mksDelay(uint16_t time);

//установки для вывода термодатчика
#define DQ_IN()  (GPIOC->MODER &= ~0x300) //  Input mode (reset state)
#define DQ_OUT()	(GPIOC->MODER |= 0x100) //  General purpose output mode.
#define S_DQ()   (GPIOC->BSRRL = GPIO_Pin_4) //set pin
#define C_DQ()   (GPIOC->BSRRH  = GPIO_Pin_4) //reset pin
#define DQ()     (GPIOC->IDR & GPIO_Pin_4) //check pin

void dpg2_readValue(uint8_t numChanel, ResultMes *result)
{
	S_DQ();
	//подадим землю
	gnd500mVOff();
	//подадим питание
	powerDa17_16(P_3_0);
	powerDa12_15(numChanel);
	//конфигурируем ногу PC4 для работы с 1-Wire
	switchOn(numChanel);
	vTaskDelay(300);
	init_Dtg2();
	vTaskDelay(1);
	uint32_t data = readDtg();
	switchOn(100);
	powerDa17_16(P_OFF);
	int16_t wTemp = data & 0xffff;
	int16_t wHumi = (data >> 16) & 0xffff;
	float fTemp=(175.72/1024/16)*wTemp-46.85;
	float fHumi=(125.0/1024/4)*wHumi-6;
	result->p = fTemp;
	result->u = fHumi;
	memcpy((void*)&result->uClear, (void*)&data, 4);
}

uint32_t readDtg()
{
	enterCritSect();
	mksDelay(100);
	uint32_t i = 0, data = 0;
	DQ_OUT();
	for(i = 0; i < 32; i++)
	{
		C_DQ();
		data >>= 1;
		mksDelay(100);
		DQ_IN();
		mksDelay(100+75);
		if( DQ() != 0)
			data |= 0x80000000;
		mksDelay(75 + 100);
		DQ_OUT();
		S_DQ();
	}
	exitCritSect();
	return data;
}

//----------------------------------------------------------------
// Start transaction with 1-wire line.
void init_Dtg2()
{
	DQ_OUT();
	C_DQ();
	vTaskDelay(11);
	S_DQ();
	DQ_IN();
	enterCritSect();
	mksDelay(80);
	exitCritSect();


	//while(DQ() != 0)
	//	; //wait for DQ low


	vTaskDelay(4);
	enterCritSect();
	DQ_OUT();
	S_DQ();
	exitCritSect();
	mksDelay(300);
}
