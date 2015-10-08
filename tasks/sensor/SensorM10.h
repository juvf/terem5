/*
 * SensorM10.h
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#ifndef SENSORM10_H_
#define SENSORM10_H_

#include "Sensor.h"
#include "stm32f4xx.h"

float MM10_Length(float curU, float a0);
float MM20_Length(float curU, float a0);
float MM50_Length(float curU, float a0);
float Relocate_Length(float curU, float *a0);



#endif /* SENSORM10_H_ */
