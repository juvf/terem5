/*
 * adc.cpp
 *
 *  Created on: 18 ���. 2015 �.
 *      Author: juvf
 */
#include "adc.h"

#define csOn()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define csOff()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

//������� ���������� ������� 11: 12.5 ��, 160 �� (���������� 50-60 �� 66 ��)
//                            9: 16,7 ��, 120 �� (���������� 50 �� 80 ��)
//                            1: 500 ��, 4 ��
#define FS  9

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

//������������� ---------------------------------------------------------------
//�����: 0-�� ��� ��� ������ ��� ���������� AD7792
uint8_t initAdc()
{
	csOn();
	//����� "����� ������ �������������" - 32 ���� 1
	SPI_I2S_SendData(SPI2, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI2, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI2, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI2, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;

	//������ �������� ������������� (�.�. 0xXA)
	uint8_t regId = AD7792Rd(ID);
	if((regId & 0x0F) != 0x0A)
	{              //������, �� ��� �����
		csOff();
		return regId;
	}

	//����� Idle-������ ������
	AD7792WrW(MODE,           //������� ������ ������
			(MR13_MD_3 * 2) |       //Idle
					(MR6_CLK_2 * 0) |       //���������� ������������
					(MR0_FS_4 * FS)         //������� ���������� �������
					);

	csOff();
	return 0;
}

void AD7792WrW(uint8_t reg, uint16_t data)
{         //reg - ����� ������������� ��������
	SPI_I2S_SendData(SPI1, reg * CR3_RS_3);  //������ � ���������������� �������
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	//������ ������ � ������ �������
	SPI_I2S_SendData(SPI1, (uint8_t)(data >> 8));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, (uint8_t)data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
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
	return SPI_I2S_ReceiveData(SPI1);
}

//��������� -------------------------------------------------------------------
//�����: ���������� �������� � �����
//#pragma inline  777 ��������� ������
uint16_t AD7792Measure(void)
{
	AD7792WrW(MODE,           //������� ������ ������
			(MR13_MD_3 * 1) |       //����������� �������������� (1)
					(MR6_CLK_2 * 0) |       //���������� ������������
					(MR0_FS_4 * FS)         //������� ���������� �������
					);
	//���� ����������, ������ ��������� � �����
	//ReadyWait();
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
	SPI_I2S_SendData(SPI1, (CR6_RW * 1) | (Register * CR3_RS_3));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	data[0] = SPI_I2S_ReceiveData(SPI1);
	SPI_I2S_SendData(SPI1, (CR6_RW * 1) | (Register * CR3_RS_3));
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	data[1] = SPI_I2S_ReceiveData(SPI1);
	return (data[0] << 8) | data[1];
}

