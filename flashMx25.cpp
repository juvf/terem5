/*
 * flashMx25.cpp
 *
 *  Created on: 07 авг. 2015 г.
 *      Author: anisimov.e
 */

#include "flashMx25.h"
#include "stm32f4xx.h"
#include "tasks/Process.h"
#include <string.h>
// etaoinsrhldcumfpgwybvkxjqz1234567890! @.?# brust

#define SIZE_BUF_FLASH	(1 + 3 + 256)

//в headerList[] хранится адреса секторов во флешь с заголовками (с началом) записанных процессов
//если 0xffff - то сектор пустой
#define MAX_SECTORS	2000
uint16_t headerList[MAX_SECTORS] = { 0xffff }; //
uint16_t countProc = 0; //кол-во процессов в флешке

uint8_t flashBuffIn[SIZE_BUF_FLASH];
uint8_t flashBuffOut[SIZE_BUF_FLASH];

uint8_t spi2Work = 0;

uint32_t currentAddress = 0; //текущий адрес, куда можно записать новый процесс

void flashMx25Write(uint8_t *source, uint32_t adrDestination)
{
	spiWREN();
	flashBuffOut[0] = 2; //Command Page Programm
	flashBuffOut[1] = adrDestination >> 16;
	flashBuffOut[2] = adrDestination >> 8;
	flashBuffOut[3] = adrDestination;
	//memcpy((void*)(flashBuffOut + 4), (void*)source, 256);
	for(int i = 0; i < 256; i++)
		flashBuffOut[3 + i] = i;

	startSpi(SIZE_BUF_FLASH);
	spiWait();
}

void flashMx25Read(void *destination, uint32_t adrSource, uint16_t size)
{
	if(size > SIZE_BUF_FLASH)
		return;
	DMA_MemoryTargetConfig(DMA1_Stream3, (uint32_t)flashBuffIn, 0);
	flashBuffOut[0] = 3; //Command Read
	flashBuffOut[1] = adrSource >> 16;
	flashBuffOut[2] = adrSource >> 8;
	flashBuffOut[3] = adrSource;
	startSpi(size);
	spiWait();
	memcpy((void*)(flashBuffIn + 4), destination, size);
}

void flashMx25ReadData(uint8_t *destination, uint32_t adrSource, uint16_t size)
{
	if(size > 4096)
		return;
	DMA_MemoryTargetConfig(DMA1_Stream3, (uint32_t)destination, 0);
	flashBuffOut[0] = 3; //Command Read
	flashBuffOut[1] = adrSource >> 16;
	flashBuffOut[2] = adrSource >> 8;
	flashBuffOut[3] = adrSource;
	startSpi(size);
	spiWait();
}

uint16_t spiRDSR()
{
//	flashBuffOut[0] = 0x05;
	setSpiOut(0, 0x05);
	setSpiOut(1, 0x00);
	setSpiOut(2, 0x00);
	startSpi(3);
	spiWait();
	uint16_t status = flashBuffIn[1];
	status <<= 8;
	status |= flashBuffIn[2];
	return status;
}

void spiSector4kErase(uint16_t numSector)
{
	spiWREN();
	flashBuffOut[0] = 0x20;
	flashBuffOut[1] = numSector >> 16;
	flashBuffOut[2] = numSector >> 8;
	flashBuffOut[3] = numSector;
	startSpi(4);
	spiWait();
}

void spiWREN()
{
	flashBuffOut[0] = 0x06;
	startSpi(1);
	spiWait();
}

void initSpi2()
{
	GPIO_InitTypeDef gpio;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &gpio);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_5;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOC, &gpio);
	csOff();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
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

	SPI_Init(SPI2, &spiInit);
	SPI_Cmd(SPI2, ENABLE);

}

void initDmaSpi2()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	//TX
	DMA_InitTypeDef dmaInit;
	dmaInit.DMA_BufferSize = 2;
	dmaInit.DMA_Channel = DMA_Channel_0;
	dmaInit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	dmaInit.DMA_Memory0BaseAddr = (uint32_t)flashBuffOut;
	dmaInit.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_Mode = DMA_Mode_Normal;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_Priority = DMA_Priority_Medium;

	DMA_Init(DMA1_Stream4, &dmaInit);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

	//RX
	dmaInit.DMA_BufferSize = 2;
	dmaInit.DMA_Channel = DMA_Channel_0;
	dmaInit.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dmaInit.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dmaInit.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	dmaInit.DMA_Memory0BaseAddr = (uint32_t)flashBuffIn;
	dmaInit.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_Mode = DMA_Mode_Normal;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(SPI2->DR);
	dmaInit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_Priority = DMA_Priority_Medium;

	DMA_Init(DMA1_Stream3, &dmaInit);
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
}

void startSpi(uint16_t number)
{
	DMA_SetCurrDataCounter(DMA1_Stream3, number);
	DMA_SetCurrDataCounter(DMA1_Stream4, number);

	csOn();
	spi2Work = 1;
	DMA_Cmd(DMA1_Stream3, ENABLE);
	DMA_Cmd(DMA1_Stream4, ENABLE);
}

void spiWait()
{
	while(spi2Work)
		;
}

void setSpiOut(uint16_t adr, uint8_t data)
{
	if(adr < SIZE_BUF_FLASH)
		flashBuffOut[adr] = data;
}


extern "C" void DMA1_Stream3_IRQHandler() //RX
{
	if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) == SET)
	{
		DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
		csOff();
		spi2Work = 0;
	}
}

extern "C" void DMA1_Stream4_IRQHandler() //TX
{
	if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET)
	{
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
	}
}

void getFirstLastProcess(uint32_t *firstHeader, uint32_t *lastFreeHeader)
{
//	firstHeader = 0;
//	curSector = 0;
//	bool isFind = false;
//	bool isFindFree = false;
//	while()
//	{
//		readHeader();
//		if(crc == 0)
//		{
//			curSector += size;
//			if(!isFind)
//				isFind = true;
//		}
//		else
//		{
//			if(isFind)
//			{
//				if(!isFindFree)
//				{
//					isFindFree = true;
//					lastFreeHeader = curSector;
//				}
//			}
//			curSector++;
//		}
//	}
}

void spiChipErase()
{
	spiWREN();
	flashBuffOut[0] = 0x60;
	startSpi(1);
	spiWait();
}

void readFlash(uint32_t adrInFlash, uint8_t *distanation, uint16_t size)
{

}

//сканирование флешки и заполненеи массива указателей заголовков процесса headerList[]
void scanProc()
{
	countProc = 0;
	HeaderProcess header;
	for(int i = 0; i < MAX_SECTORS; i++)
	{
		headerList[i] = 0xffff;
		flashMx25Read((void*)&header, i * 4096, sizeof(HeaderProcess));
		if(headerIsValid(header))
		{
			headerList[countProc++] = i;
		}
	}
}

void findBeginEndFreeMem(uint32_t *beginSector, uint32_t *endSector)
{
//	uint32_t curSector = 0;
//	while()
//	{
//		readHeader();
//		if(header == GOOD)
//		{//нашли первый годный заголовок. находим след свободное место.
//			curSector += size;
//			if(fBegin)
//			{//помечаем последний свободный сектор
//				break;//выходим из вайла
//			}
//		}
//		else
//		{
//			curSector++;
//			if(!fBegin)
//			{//помечаем первый свободный сектор
//				fBegin = true;
//			}
//		}
//
//	}
}

