/*
 * Sensor.h
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_H_
#define TASKS_SENSOR_H_

#include <stdint.h>

#define gnd500mVOn()	GPIO_ResetBits(GPIOC, GPIO_Pin_9)
#define gnd500mVOff()	GPIO_SetBits(GPIOC, GPIO_Pin_9)

enum { P_OFF = 0, P_1_67, P_ADC_REF, P_3_0, P_EP1};

float readSenser(uint8_t numChanel);
void switchOn(uint8_t numChanel);
void powerDa17_16(uint8_t val);
void powerDa12_15(uint8_t val);
float readSensorM(uint8_t numChanel);
float readAnalogSensor(uint8_t numChanel);

#endif /* TASKS_SENSOR_H_ */
