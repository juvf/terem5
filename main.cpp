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
	initRtc();

	//usbTask(0);

	initOs();

	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);

	vTaskStartScheduler();
	return 0;
}

void pereferInit()
{
	// �������� ����������
	__enable_irq();
//������������� ����������
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	GPIO_InitTypeDef port;
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_10;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	//������������� GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOBEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_10;
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &port);

	GPIO_SetBits(GPIOB, GPIO_Pin_10); //1-Wire �������� � "1"

	//������������� GPIOC
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12
			| GPIO_Pin_8 | GPIO_Pin_2 | GPIO_Pin_0; //��� ��� ��������� �����/������ �� 8 ������� ��������
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);
	GPIO_SetBits(GPIOC, GPIO_Pin_9);	//�������� 500 ��

	GPIO_StructInit(&port);
	port.GPIO_Pin = GPIO_Pin_4;	//��� ��� ��������� �����/������ �� 8 ������� ��������
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_OType = GPIO_OType_PP;
	port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &port);

	//������������� GPIOD
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

	//������������� GPIOE
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

	initSpi2();
	initDmaSpi2();

	initSpi1();
	initIntAdc();
}

void pereferDeInit()
{
	//deinitUartRfd();

	ADC_DeInit();
	I2C_DeInit(I2C1);
	SPI_I2S_DeInit(SPI1);
	SPI_I2S_DeInit(SPI2);
	DMA_DeInit(DMA1_Stream3);
	DMA_DeInit(DMA1_Stream4);
	deinitGPIO();
}

void deinitGPIO()
{
//	GPIO_DeInit(GPIOA);
//	GPIO_DeInit(GPIOB);
//	GPIO_DeInit(GPIOC);
//	GPIO_DeInit(GPIOD);
//	GPIO_DeInit(GPIOE);
	//�������� ����� �� ���������� ������������ DA12-DA17

	GPIO_DeInit(GPIOD);

	GPIO_InitTypeDef gpio;

	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | //SPI ADC
			GPIO_Pin_1; // ���������� ������ DS1820 ��� ������ ��������� SHT21D
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_6
			| GPIO_Pin_8 | GPIO_Pin_9 //i2c
			| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 //spi mx25l64
			| GPIO_Pin_10; //���������� ������ DS1820 ��� ������ ��������� SHT21D
	GPIO_Init(GPIOB, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9
			| GPIO_Pin_5; //spi mx25l64
	GPIO_Init(GPIOC, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1
			| GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; //RFM23B-433
	GPIO_Init(GPIOE, &gpio);
}

