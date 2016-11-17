/*
 * i2c.cpp
 *
 *  Created on: 04 авг. 2015 г.
 *      Author: juvf
 */
#include "i2c.h"
#include "stm32f4xx_conf.h"
#include "osConfig.h"
#include "tasks/CritSect.h"

static bool isInitI2C = false;

void init_I2C1(int u)
{
	if( !isInitI2C || (u == 0) )
	{
		GPIO_InitTypeDef gpio;
		I2C_InitTypeDef i2c;
		// Включаем тактирование нужных модулей
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		// I2C использует две ноги микроконтроллера, их тоже нужно настроить
		gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_OType = GPIO_OType_OD;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &gpio);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		// А вот и настройка I2C
		i2c.I2C_ClockSpeed = 100000;
		i2c.I2C_Mode = I2C_Mode_I2C;
		i2c.I2C_DutyCycle = I2C_DutyCycle_2;
		// Адрес я тут взял первый пришедший в голову
		i2c.I2C_OwnAddress1 = 0x01;
		i2c.I2C_Ack = I2C_Ack_Enable;
		i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		//Ну и включаем, собственно, модуль I2C1
		I2C_Init(I2C1, &i2c);
		I2C_Cmd(I2C1, ENABLE);
		//I2C_AcknowledgeConfig(I2C1, ENABLE);
		isInitI2C = true;
	}
}

void i2cWrite(int slaveAdr, int address, uint8_t *buffer, int size)
{
	if( isInitI2C )
		init_I2C1(1);
	while(size > 0)
	{
		int adrNextPage = 64 + (address & 0xffc0);
		int pageSize = adrNextPage - address;
		if( size > pageSize )
		{
			i2cWritePage(slaveAdr, address, buffer, pageSize);
			vTaskDelay(10);
			size -= pageSize;
			address += pageSize;
			buffer += pageSize;
		}
		else
		{
			i2cWritePage(slaveAdr, address, buffer, size);
			vTaskDelay(10);
			size = 0;
		}
	}
}

//в этой функции запись должна идти до конца страницы, например до 64
void i2cWritePage(int slaveAdr, int address, uint8_t *buffer, int size)
{
	if( isInitI2C )
		init_I2C1(1);
	// Ждем пока шина освободится
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		;
	// Генерируем старт
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
		;
	// Посылаем адрес подчиненному устройству
	I2C_Send7bitAddress(I2C1, slaveAdr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		;
	//старший адресс
	I2C_SendData(I2C1, address >> 8);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
		;

	//младший адресс
	I2C_SendData(I2C1, address & 0xff);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
		;

	int currentBytesValue = 0;
	// И наконец отправляем наши данные
	while(currentBytesValue < size)
	{
		I2C_SendData(I2C1, buffer[currentBytesValue++]);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
			;
	}

	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF))
		;

	I2C_GenerateSTOP(I2C1, ENABLE);
}

void i2cRead(int slaveAdr, int address, uint8_t *buffer, int size)
{
	if( !isInitI2C )
		init_I2C1(1);
	// Ждем пока шина освободится
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		;
	// Генерируем старт
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
		;
	// Посылаем адрес подчиненному устройству - микросхеме CS43L22
	I2C_Send7bitAddress(I2C1, slaveAdr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		;
	//старший адресс
	I2C_SendData(I2C1, address >> 8);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
		;

	//младший адресс
	I2C_SendData(I2C1, address & 0xff);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
		;

	// Генерируем старт
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
		;
	// Посылаем адрес подчиненному устройству - микросхеме CS43L22
	I2C_Send7bitAddress(I2C1, slaveAdr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		;

	int currentBytesValue = 0;
	// И наконец получаем наши данные
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	do
	{
		if( (size - currentBytesValue) == 1 )
			I2C_AcknowledgeConfig(I2C1, DISABLE);
		while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE))
			;
		buffer[currentBytesValue++] = I2C_ReceiveData(I2C1);
	} while(currentBytesValue < size);

	I2C_GenerateSTOP(I2C1, ENABLE);
}

void deinit_I2C1()
{
	I2C_GenerateSTOP(I2C1, ENABLE);
	vTaskDelay(10);
	I2C_DeInit(I2C1);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &gpio);

	isInitI2C = false;
}
