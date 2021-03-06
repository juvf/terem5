/*
 * adc.h
 *
 *  Created on: 18 ���. 2015 �.
 *      Author: juvf
 */

#ifndef ADC_H_
#define ADC_H_
#include "stm32f4xx.h"
#include <stdint.h>

#ifdef __cplusplus


/************************************************************
* STANDARD BITS
************************************************************/

#define BIT0                (0x0001u)
#define BIT1                (0x0002u)
#define BIT2                (0x0004u)
#define BIT3                (0x0008u)
#define BIT4                (0x0010u)
#define BIT5                (0x0020u)
#define BIT6                (0x0040u)
#define BIT7                (0x0080u)
#define BIT8                (0x0100u)
#define BIT9                (0x0200u)
#define BITA                (0x0400u)
#define BITB                (0x0800u)
#define BITC                (0x1000u)
#define BITD                (0x2000u)
#define BITE                (0x4000u)
#define BITF                (0x8000u)


//#define                 //���������������� �������
#define   CR7_WEN   BIT7  //���������� ������ (0)
#define   CR6_RW    BIT6  //������ (1) / ������ (0)
#define   CR3_RS_3  BIT3  //����� �������� (3 ����)
                            //0-����������������/�������, 8 ���
                            //1-Mode, 16 ���
                            //2-Configuration, 16 ���
                            //3-Data, 16 ���
                            //4-ID, 8 ���
                            //5-IO, 8 ���
                            //6-Offset, 16 ���
                            //7-Full scale, 16 ���
#define   CR2_CREAD BIT2  //����� ���������������� ������ (1)

//������� ������� (����)
#define SR          0
#define SR7_RDY     BIT7    //���������� (0) ������
#define SR6_ERR     BIT6    //������������ (1) ������ (��� 0 ��� ��� 1)
#define SR3_7792    BIT3    //��� AD7792 ������ 0
#define SR0_CH_3    BIT0    //����� (3 ����)

//������� ������ ������ (�����)
#define MODE        1
#define MR13_MD_3   BITD    //����� ������ (3 ����)
                              //0-����������� ��������������
                              //1-����������� ��������������
                              //2-Idle
                              //3-Power Down
                              //4-���������� ���������� ����
                              //5-���������� ���������� ������ �����
                              //6-��������� ���������� ����
                              //7-��������� ���������� ������ �����
#define MR6_CLK_2   BIT6    //����� ������������ (2 ����)
                              //0-���������� 64 ���
                              //1-�� �� � ������� ������
                              //2-������� 64 ���
                              //3-������� ������������ � �������� /2
#define MR0_FS_4    BIT0    //������� ���������� ������� (4 ����)
                              //0-�� ������������
                              //1-500 ��, 4 ��
                              //2-250 ��, 8 ��
                              //3-125 ��, 16 ��
                              //4-62,5 ��, 32 ��
                              //5-50 ��, 40 ��
                              //6-39,2 ��, 48 ��
                              //7-33,3 ��, 60 ��
                              //8-19,6 ��, 101 �� (���������� 60 �� 90 ��)
                              //9-16,7 ��, 120 �� (���������� 50 �� 80 ��)
                              //10-16,7 ��, 120 �� (���������� 50-60 �� 65 ��)
                              //11-12,5 ��, 160 �� (���������� 50-60 �� 66 ��)
                              //12-10 ��, 200 �� (���������� 50-60 �� 69 ��)
                              //13-8,33 ��, 240 �� (���������� 50-60 �� 70 ��)
                              //14-6,25 ��, 320 �� (���������� 50-60 �� 72 ��)
                              //15-4,17 ��, 480 �� (���������� 50-60 �� 74 ��)
//������� ������������ (�����)
#define CON         2
#define CON14_VBIAS_2 BITE  //���������� ���������� ��������
                              //0-��������
                              //1-���������� � AIN1(-)
                              //2-���������� � AIN2(-)
                              //3-������
#define CON13_B0    BITD  //���������� 100 �� ��������� ���� (1)
#define CON12_UB    BITC  //������������ (1) / ���������� (0) �����
#define CON11_BOOST BITB  //��������� ��������� ���������� �������� (1)
#define CON8_G_3    BIT8  //��������
                            //0-1 (����� 2,5 � ��� 1,17 �)
                            //1-2 (1,25 � ��� 585 ��)
                            //2-4 (625 �� ��� 292,5 ��)
                            //3-8 (312,5 �� ��� 146,25 ��)
                            //4-16 (156,2 �� ��� 73,125 ��)
                            //5-32 (78,125 �� ��� 36,5625 ��)
                            //6-64 (39,06 �� ��� 18,28125 ��)
                            //7-128 (19,53 �� ��� 9,140625 ��)
#define CON7_REFSEL BIT7  //������� (0) / ���������� (1) ������� ����������
#define CON4_BUF    BIT4  //������������� (1) �������� �������
#define CON0_CH_3   BIT0  //����� ������ ��������� (3 ����)
                            //0-AIN1(+)-AIN1(-)
                            //1-AIN2(+)-AIN2(-)
                            //2-AIN3(+)-AIN3(-)
                            //3-AIN1(-)-AIN1(-)
                            //6-������ �����������
                            //7-������� AVdd
//������� ������ (�����)
#define DATA        3

//������� ������������� (����)
#define ID          4

//������� ���������� ��������� ���� (����)
#define IO          5
#define IO2_IEXCDIR_2 BIT2  //����������� ��������� ����
                              //0-IEXC1 ��������� � IOUT1, IEXC2 - � IOUT2
                              //1-IEXC1 ��������� � IOUT2, IEXC2 - � IOUT1
                              //2-��� ��������� ���������� � IOUT1 (��� 10 ��� ��� 210 ���)
                              //3-��� ��������� ���������� � IOUT2 (��� 10 ��� ��� 210 ���)
#define IO0_IEXCEN_2 BIT0   //���������� ��������� ����
                              //0-��������
                              //1-10 ���
                              //2-210 ���
                              //3-1 ��

//������� �������� (��� ���������� ����) (�����)
#define OFFSET      6

//������� ������ ����� (��� ���������� ������ �����) (�����)
#define FULL        7

uint8_t AD7792Rd(uint8_t reg);
void AD7792WrW(uint8_t reg, uint16_t data);
uint16_t AD7792RdW(unsigned char Register);
void AD7792Wr(unsigned char Register, unsigned char Data);
uint16_t AD7792Measure();
//float getU_Ad7792(unsigned char *curRange, unsigned char type);
float getU_Ad7792(unsigned char numChanel, uint16_t *code = 0);
void IO_420();
void AD7792Calibr();
void AD7792Calibr7();
void IO_Off();
float GainKoef(unsigned char Range);


//--- ������� �� ������ � ���������� ���
void initIntAdc();
void deinitInAdc();
uint16_t getBatValue();

extern "C"
{
#endif
void initSpi1();
uint8_t initAdc();
void spiPortAdcOn();
void spiPortAdcOff();
void ledError(int kl);

#ifdef __cplusplus
}
#endif


#endif /* ADC_H_ */
