/*
 * adc.cpp
 *
 *  Created on: 18 рту. 2015 у.
 *      Author: juvf
 */
#include "adc.h"

#define csOn()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define csOff()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

void initSpi1()
{
	GPIO_InitTypeDef gpio;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
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

