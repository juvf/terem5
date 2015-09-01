#include "main.h"
#include "osConfig.h"
#include "stm32f4xx_conf.h"
#include "tasks/taskUartRfd.h"
#include "tasks/clock.h"
#include "i2c.h"
#include "tasks/configTerem.h"
#include "flashMx25.h"
#include "tasks/Process.h"
#include "adc.h"
#include <string.h>

int main()
{
	pereferInit();

	initOs();

	initConfigTerem();

	initListProc();

	static int as = sizeof(TeremConfig);
	as++;


	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);

	vTaskStartScheduler();
	return 0;
}

void pereferInit()
{
	// Включаем прерывания
	__enable_irq();

	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	GPIO_InitTypeDef port;
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	//инициализация GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_7;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &port);

	//инициализация GPIOC
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_8;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);//выключим 500 мВ

	//инициализация GPIOD
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIODEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_5 |
			GPIO_Pin_7;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &port);

	//инициализация GPIOE
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOEEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &port);

	//initUartForConsol();

	initUartRfd();

	initRtc();

	init_I2C1();

	initSpi2();
	initDmaSpi2();

	initSpi1();
	if(initAdc() != 0 )
		while(1);

//	setSpiOut(0, 0x9f);
//	setSpiOut(1, 0x00);
//	setSpiOut(2, 0x00);
//	setSpiOut(3, 0x00);
//	startSpi(4);
//	spiWait();
//
//	spiRDSR();
//
//	spiSector4kErase(0);
//
//
//	flashMx25Write(0, 0);
//	flashMx25Read(0, 0);

	asm("nop");

//	static uint8_t data[4];
//	csOn();
//	SPI_I2S_SendData(SPI2, 0x9f);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
//	data[0] = SPI_I2S_ReceiveData(SPI2);
//	SPI_I2S_SendData(SPI2, 0x00);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
//	data[0] = SPI_I2S_ReceiveData(SPI2);
//	SPI_I2S_SendData(SPI2, 0x00);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
//	data[1] = SPI_I2S_ReceiveData(SPI2);
//	SPI_I2S_SendData(SPI2, 0x00);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
//	data[3] = SPI_I2S_ReceiveData(SPI2);
//
//	csOff();

}

void initUartForConsol()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	// Инициализация нужных пинов контроллера, для USART1 –
	// PA9 и PA10
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio);

	// И вот еще функция, которой не было при работе с STM32F10x,
	// но которую нужно вызывать при использовании STM32F4xx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// А теперь настраиваем модуль USART
	USART_StructInit(&usart);
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_BaudRate = 115200;
	USART_Init(USART2, &usart);

	// Включаем прерывания и запускаем USART
	//NVIC_EnableIRQ(USART1_IRQn);
	USART_Cmd(USART2, ENABLE);
}

