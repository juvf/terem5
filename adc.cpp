/*
 * adc.cpp
 *
 *  Created on: 18 ���. 2015 �.
 *      Author: juvf
 */
#include "adc.h"
#include "structCommon.h"
#include "tasks/configTerem.h"

#define csOn()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define csOff()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

//����������� ������� �������� ���������� (���� �� MISO)
#define IsMISOSet() (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) != 0)
#define ReadyWait()  while(IsMISOSet())

//������� ���������� ������� 11: 12.5 ��, 160 �� (���������� 50-60 �� 66 ��)
//                            9: 16,7 ��, 120 �� (���������� 50 �� 80 ��)
//                            1: 500 ��, 4 ��
#define FS  9

unsigned char CurRangeADC = 0;
uint16_t CalFull_1;  //�������� ������������� �������������

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
	SPI_I2S_SendData(SPI1, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, 0xff);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, 0xff);
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
uint16_t AD7792Measure()
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

//=============================================================================
//��������� ���������� � ���������������� ��������� ---------------------------
float getU_Ad7792(unsigned char numChanel)
{
	uint8_t *CurRange = &configTerem.adcRange[numChanel];
	uint16_t CurCode;               //������� �������� � �����
	float curU;                 //������� �������� ���������� � �������

	//��� HEL700
	if(configTerem.sensorType[numChanel] == GT_HEL700)
	{
		csOn();  //������ Chip Select
		IO_420();     //��������� ���� 2*210 ��� �� IOUT2
		//�������� ��������� (-50..+270)�� ������������� �������� ��������� 2 ���
		AD7792WrW(CON,            //������ � ������� ������������
				(CON14_VBIAS_2 * 0) |   //���������� �������� �� �������� (0)
						(CON13_B0 * 0) |   //�������� ���� 100 �� �������� (0)
						(CON12_UB * 1) |   //������������ ����� (1)
						(CON11_BOOST * 0) | //��������� ��������� ���������� �������� ��������� (0)
						(CON8_G_3 * 0) |   //�������� 1 (0)
						(CON7_REFSEL * 0) |   //������� ������� (0)
						(CON4_BUF * 1) | //������������� �������� ������� �������� (1)
						(CON0_CH_3 * 0)         //����� AIN1 (0)
						);
		//���������� ������ 1 (������, �.�. ����� ���� ������ � �����������)
		AD7792Calibr();
		csOff(); //������ Chip Select
		CurRangeADC = 0xFF;
		//���� ������������� ������� � ����, �� ����������
		//if(CalFull_1 <= 0x100) {U[Ad][Channel]=flErrorCode; continue;}
		//���������
		csOn();
		CurCode = AD7792Measure();
		IO_Off(); //��������� ���� ���������
		csOff();
		if((CurCode == 0) || (CurCode == 0xFFFF))
			//gFlags.BadResult = 1;
			;
		else //           ���   2��� 16 ���
		{
			curU = CurCode * (2.0 / 0xFFFF);
			//gFlags.BadResult = 0;
		}
		//Status=AD7792Rd(SR);

		/*
		 //��� �������������
		 }else if((Type>=GT_TensoKg) && (Type<=GT_TensoMPa)) {
		 IO_Off(); //��������� ���� ���������
		 AD7792WrW(CON,            //������ � ������� ������������
		 (CON14_VBIAS_2 * 0) |   //���������� �������� �� �������� (0)
		 (CON13_B0 * 0)      |   //�������� ���� 100 �� �������� (0)
		 (CON12_UB * 0)      |   //����������� ����� (0)
		 (CON11_BOOST * 0)   |   //��������� ��������� ���������� �������� ��������� (0)
		 (CON8_G_3 * 7)      |   //�������� 128
		 (CON7_REFSEL * 1)   |   //������� ���������� (1)
		 (CON4_BUF * 1)      |   //������������� �������� ������� �������� (1)
		 (CON0_CH_3 * 0)         //����� AIN1 (0)
		 );
		 //���������� ������ 1
		 if(CurRangeADC != 7) {AD7792Calibr(); CurRangeADC=7;}
		 //���������
		 CurCode=AD7792Measure();
		 //������� �������� ���������� � ������� (����������� �����)
		 U[Ad][Channel] = (CurCode*(1.17/128/0x8000)-(1.17/128));
		 */
	}
	else
	{//��� ��������� ����� ��������
		csOn();
		IO_Off(); //��������� ���� ���������
		csOff();
		//���� ��������� � ���������������� ���������
		while(1)
		{
			csOn();
			AD7792WrW(CON,            //������ � ������� ������������
					(CON14_VBIAS_2 * 0) | //���������� �������� �� �������� (0)
							(CON13_B0 * 0) | //�������� ���� 100 �� �������� (0)
							(CON12_UB * 0) | //����������� ����� (0)
							(CON11_BOOST * 0) | //��������� ��������� ���������� �������� ��������� (0)
							(CON8_G_3 * *CurRange) | //��������
							(CON7_REFSEL * 1) | //������� ���������� (1)
							(CON4_BUF * 1) | //������������� �������� ������� �������� (1)
							(CON0_CH_3 * 0)         //����� AIN1 (0)
							);
			//���������� ������ 1
			if(CurRangeADC != *CurRange)
			{
				CurRangeADC = *CurRange;
				if(*CurRange != 7)
					AD7792Calibr();
				else
					AD7792Calibr7();
				csOff();
			}
			//���������
			csOn();
			CurCode = AD7792Measure();
			csOff();
			//���������� (+), ��������� ����������� �������� PGA
			if(CurCode == 0xFFFF)
			{
				if(*CurRange)
				{
					if(--(*CurRange))
						(*CurRange)--;
					//gFlags.RangeChanged = 1;
				}
				else if((configTerem.sensorType[numChanel] >= GT_MM10) && (configTerem.sensorType[numChanel] <= GT_Rel_Ind))
				{
					//gFlags.BadResult = 0;
					break;
				}
				else
				{
					//gFlags.BadResult = 1;
					break;
				}
				//���������� (-), ��������� ����������� ��������
			}
			else if(!CurCode)
			{
				if(*CurRange)
				{
					if(--(*CurRange))
						(*CurRange)--;
					//gFlags.RangeChanged = 1;
				}
				else
				{
					//gFlags.BadResult = 1;
					break;
				}
				//������������� ������������� �����������
			}
			else if((CurCode < 0x8800) && (CurCode > 0x7800) && (*CurRange < 7))
			{
				//��������� ����������� �������� PGA ��� ���������� ��������
				while((CurCode < 0x8800) && (CurCode > 0x7800)
						&& (*CurRange < 7))
				{
					(*CurRange)++;
					//gFlags.RangeChanged = 1;
					CurCode = (((signed)(CurCode - 0x8000)) * 2) + 0x8000;
				}
				//��������� �����������
			}
			else
			{
				//gFlags.BadResult = 0;
				break;
			}
		}
		curU = GainKoef(*CurRange) * (int16_t)(CurCode - 0x8000);
	}
	return curU;
}

//���/���� ���������� ���� ----------------------------------------------------
void IO_420()
{ //2*210=420 ���
	AD7792Wr(IO,              //������� IO
			(IO2_IEXCDIR_2 * 3) |   //����������� IEXC1, IEXC2 � IOUT2
					(IO0_IEXCEN_2 * 2) //2 * 210 ��� (0.84 � �� �����, �� 0.75 �� HEL700 - 200�C)
					);
}

//��������� � ��������� AD7792 ------------------------------------------------
void AD7792Wr(unsigned char Register, unsigned char Data)
{
	//ShiftSPIw(((Register * CR3_RS_3) << 8) + Data);

	SPI_I2S_SendData(SPI1, Register * CR3_RS_3);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
	SPI_I2S_SendData(SPI1, Data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		;
}

//���������� ------------------------------------------------------------------
void AD7792Calibr()
{
	AD7792WrW(MODE,           //������� ������ ������
			(MR13_MD_3 * 5) |       //���������� ���������� ������ �����
					(MR6_CLK_2 * 0) |       //���������� ������������
					(MR0_FS_4 * FS)         //������� ���������� �������
					);
	//���� ����������, ������ ���������
	ReadyWait()
		;
	CalFull_1 = AD7792RdW(FULL);
}

void IO_Off()
{ //����
	AD7792Wr(IO,              //������� IO
			(IO2_IEXCDIR_2 * 2) |   //����������� IEXC1, IEXC2 � IOUT1
					(IO0_IEXCEN_2 * 0)     //���������
					);
}

void AD7792Calibr7()
{
	//��������� ������� � �������� 6
	AD7792WrW(CON,            //������ � ������� ������������
			(CON14_VBIAS_2 * 0) |   //���������� �������� �� �������� (0)
					(CON13_B0 * 0) |   //�������� ���� 100 �� �������� (0)
					(CON12_UB * 0) |   //����������� ����� (0)
					(CON11_BOOST * 0) | //��������� ��������� ���������� �������� ��������� (0)
					(CON8_G_3 * 6) |   //��������
					(CON7_REFSEL * 1) |   //������� ���������� (1)
					(CON4_BUF * 1) | //������������� �������� ������� �������� (1)
					(CON0_CH_3 * 0)         //����� AIN1 (0)
					);
	AD7792WrW(MODE,           //������� ������ ������
			(MR13_MD_3 * 5) |       //���������� ���������� ������ �����
					(MR6_CLK_2 * 0) |       //���������� ������������
					(MR0_FS_4 * FS)         //������� ���������� �������
					);
	//���� ����������, ������ ���������
	ReadyWait()
		;
	CalFull_1 = AD7792RdW(FULL);
	//������� � �������� 7
	AD7792WrW(CON,            //������ � ������� ������������
			(CON14_VBIAS_2 * 0) |   //���������� �������� �� �������� (0)
					(CON13_B0 * 0) |   //�������� ���� 100 �� �������� (0)
					(CON12_UB * 0) |   //����������� ����� (0)
					(CON11_BOOST * 0) | //��������� ��������� ���������� �������� ��������� (0)
					(CON8_G_3 * 7) |   //��������
					(CON7_REFSEL * 1) |   //������� ���������� (1)
					(CON4_BUF * 1) | //������������� �������� ������� �������� (1)
					(CON0_CH_3 * 0)         //����� AIN1 (0)
					);
}

//��������� ����. �������� �� ���� --------------------------------------------
float GainKoef(unsigned char Range)
{
	switch(Range)
	{
		default:
			return 1.17 / 0x7FFF;
		case 1:
			return 1.17 / 2 / 0x7FFF;
		case 2:
			return 1.17 / 4 / 0x7FFF;
		case 3:
			return 1.17 / 8 / 0x7FFF;
		case 4:
			return 1.17 / 16 / 0x7FFF;
		case 5:
			return 1.17 / 32 / 0x7FFF;
		case 6:
			return 1.17 / 64 / 0x7FFF;
		case 7:
			return 1.17 / 128 / 0x7FFF;
	}
}

