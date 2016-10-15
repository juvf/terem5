/*
 * Sensor.h
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_H_
#define TASKS_SENSOR_H_

#include <stdint.h>
#include "../../structCommon.h"

#ifdef __cplusplus

#define gnd500mVOn()	GPIO_ResetBits(GPIOC, GPIO_Pin_9)
#define gnd500mVOff()	GPIO_SetBits(GPIOC, GPIO_Pin_9)

enum { P_OFF = 0, P_1_67, P_ADC_REF, P_3_0, P_EP1, P_1_67_V_REF};

void powerDa17_16(uint8_t val);
void powerDa12_15(uint8_t numChanel);
float readSensorM(uint8_t numChanel);
float readAnalogSensor(uint8_t numChanel, uint16_t *codeN = 0);
ResultMes readSenser(uint8_t numChanel, uint16_t *codeN = 0);

float Polinom4(float x, const float *A);
float Polinom3(float x, const float *A);

extern "C"
{
#endif

void switchOn(uint8_t numChanel);

#ifdef __cplusplus
}
#endif /* TASKS_SENSOR_H_ */


#endif /* TASKS_SENSOR_H_ */
