/*
 * ds1820.h
 *
 *  Created on: 13 ���. 2015 �.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_DS1820_H_
#define TASKS_SENSOR_DS1820_H_

#include <stdint.h>

////������� ������ � microLAN, ��� ����� ��������� � ���������� DQ_uLAN
//#define uLANZ0()        PORT_uLAN &=~DQ_uLAN;                       //��������� ���������� �����
//#define uLAN0()         PORT_uLAN &=~DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //��������� ���������� ����� � ������������ �� �� �����
//#define uLANZ()         DIR_uLAN  &=~DQ_uLAN;                       //�������������� ���������� ����� (Z)
//#define uLAN1()         PORT_uLAN |= DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //��������� ���������� ����� � 1 � ����������� �� �����
//#define uLANis_1()      (PIN_uLAN &  DQ_uLAN)                       //������ ��������� �����

//#define uLAN0()         PORT_uLAN &=~DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //��������� ���������� ����� � ������������ �� �� �����

//void initDS1820();
//void uLAN_SkipROM();
//void uLAN_WrBYTE(uint8_t byte);
//uint8_t uLAN_Reset();

void init_ds18b20();
void mksDelay(uint16_t time);
float readtemp();
void writecommand(uint8_t data);
uint8_t readbyte();


#endif /* TASKS_SENSOR_DS1820_H_ */
