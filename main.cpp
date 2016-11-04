#include "main.h"
#include "osConfig.h"
#include "stm32f4xx_conf.h"
#include "tasks/taskUartRfd.h"
#include "tasks/mainTask.h"
#include "tasks/clock.h"
#include "i2c.h"
#include "tasks/configTerem.h"
#include "flashMx25.h"
#include "tasks/Process.h"
#include "tasks/usbTask.h"
#include "adc.h"
#include <string.h>

int main()
{
	pereferInit();
	initOs();

	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);

	vTaskStartScheduler();
	return 0;
}

void pereferInit()
{
	//DBGMCU->CR |= DBGMCU_CR_DBG_STOP;
	// Включаем прерывания
	__enable_irq();
//инициализация светодиода
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	GPIO_InitTypeDef port;
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_10;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	//инит ноги РА9 для отслеживания подключения УСБ
	initPa9();

	//инициализация GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOBEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_10;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &port);

	GPIO_SetBits(GPIOB, GPIO_Pin_10); //1-Wire подтянем к "1"

	//инициализация GPIOC
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12
			| GPIO_Pin_8 | GPIO_Pin_2 | GPIO_Pin_0; //пин для цыфрового входа/выхода на 8 каналов датчиков
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);	//выключим 500 мВ

	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_4;	//пин для цыфрового входа/выхода на 8 каналов датчиков
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);

	//инициализация GPIOD
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIODEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_1
			| GPIO_Pin_0 | GPIO_Pin_6 | GPIO_Pin_5 |
			GPIO_Pin_7;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &port);

	//инициализация GPIOE
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOEEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_15;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &port);

	initUartRfd();

	init_I2C1();

	//initSpi1();
	//initIntAdc();
	initRtc();
}

void pereferDeInit()
{
	//deinitUartRfd();

	ADC_DeInit();
	deinit_I2C1();
	SPI_I2S_DeInit(SPI1);
//	deinitSpi2();
	deinitGPIO();
}

void deinitGPIO()
{
//	GPIO_DeInit(GPIOA);
//	GPIO_DeInit(GPIOB);
//	GPIO_DeInit(GPIOC);
//	GPIO_DeInit(GPIOD);
//	GPIO_DeInit(GPIOE);
	//деинитим порты на управление комутаторами DA12-DA17

	GPIO_DeInit(GPIOD);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIODEN, DISABLE);

	GPIO_InitTypeDef gpio;

	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | //SPI ADC
<<<<<<< .mine
			GPIO_Pin_1; // | // внутренний датчик DS1820 или датчик влажности SHT21D
	//GPIO_Pin_10; //светодиод
=======
			GPIO_Pin_1; // датчик влажности SHT21D

>>>>>>> .theirs
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8
			| GPIO_Pin_9 //i2c
			| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 //spi mx25l64
			| GPIO_Pin_10; //внутренний датчик DS1820
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9
			| GPIO_Pin_5; //spi mx25l64
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_9 | GPIO_Pin_10
			| GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; //RFM23B-433
	GPIO_Init(GPIOE, &gpio);
}

void initAfterStop()
{
	//initUartRfd();
}

void initPa9()
{

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource9);

	EXTI_InitTypeDef exti;
	exti.EXTI_Line = EXTI_Line9;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_SetPriority(EXTI9_5_IRQn, 12);

}
