/*
 * structCommon.c
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: juvf
 */
#include "structCommon.h"
#include "adc.h"
#include "tasks/sensor/Sensor.h"
#include "main.h"
#include "osConfig.h"

extern void spiPortAdcOn();
extern void spiPortAdcOff();
extern uint8_t initAdc();

uint32_t u32FromU8(const uint8_t *buffer)
{
	uint32_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	result |= (((uint32_t)buffer[2] & 0xff) << 16);
	result |= (((uint32_t)buffer[3] & 0xff) << 24);
	return result;
}

uint64_t u64FromU8(const uint8_t *buffer)
{
	uint64_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	result |= (((uint32_t)buffer[2] & 0xff) << 16);
	result |= (((uint32_t)buffer[3] & 0xff) << 24);
	result |= (((uint32_t)buffer[4] & 0xff) << 32);
	result |= (((uint32_t)buffer[5] & 0xff) << 40);
	result |= (((uint32_t)buffer[6] & 0xff) << 48);
	result |= (((uint32_t)buffer[7] & 0xff) << 56);
	return result;
}

uint16_t u16FromU8(const uint8_t *buffer)
{
	uint32_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	return result;
}

void ep1_Off()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_8); //выключим питание Ep1

	//переведём ноги в 3-е состояние
	GPIO_InitTypeDef port;
	//порт С
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12;
	port.GPIO_Mode = GPIO_Mode_IN;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);

	//порт D
	port.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6 |
	GPIO_Pin_5 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_Init(GPIOD, &port);

	//порт E
	port.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Init(GPIOE, &port);
}

void ep1_On()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_8);

	GPIO_InitTypeDef port;
	//порт С
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);

	//порт D
	port.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6 |
	GPIO_Pin_5 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_Init(GPIOD, &port);

	//порт E
	port.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_Init(GPIOE, &port);
}

void epa_On()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2); //вкл епа

	{
		//инициализируем порты управления аналоговыми ключами выбора канала
		GPIO_InitTypeDef port;
		GPIO_StructInit(&port);
		port.GPIO_Pin = GPIO_Pin_12;
		port.GPIO_Mode = GPIO_Mode_OUT;
		port.GPIO_OType = GPIO_OType_PP;
		port.GPIO_PuPd = GPIO_PuPd_UP;
		port.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(GPIOC, &port);

		port.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
		GPIO_Init(GPIOB, &port);

		RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIODEN, ENABLE);
		port.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &port);
	}


	switchOn(100); //выключаем все ключи

	initSpi1();
	vTaskDelay(2);

	initAdc();
}

void epa_Off()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	//перевести ноги с ключа в 3-е состояние
	GPIO_InitTypeDef gpio;

	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOD, &gpio);

	SPI_I2S_DeInit(SPI1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
	spiPortAdcOff();

}

