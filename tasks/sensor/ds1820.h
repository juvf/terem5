/*
 * ds1820.h
 *
 *  Created on: 13 дек. 2015 г.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_DS1820_H_
#define TASKS_SENSOR_DS1820_H_

////Макросы работы с microLAN, бит линии определен в переменной DQ_uLAN
//#define uLANZ0()        PORT_uLAN &=~DQ_uLAN;                       //Обнуление сигнальной линии
//#define uLAN0()         PORT_uLAN &=~DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //Обнуление сигнальной линии и переключение ее на выход
//#define uLANZ()         DIR_uLAN  &=~DQ_uLAN;                       //Восстановление сигнальной линии (Z)
//#define uLAN1()         PORT_uLAN |= DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //Перевести сигнальную линию в 1 и переключить на выход
//#define uLANis_1()      (PIN_uLAN &  DQ_uLAN)                       //Чтение состояния линии

#define uLAN0()         PORT_uLAN &=~DQ_uLAN; DIR_uLAN |= DQ_uLAN;  //Обнуление сигнальной линии и переключение ее на выход

void initDS1820();
void uLAN_SkipROM();
void uLAN_WrBYTE(uint8_t byte);
uint8_t uLAN_Reset();


#endif /* TASKS_SENSOR_DS1820_H_ */
