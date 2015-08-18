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

#define gnd500mVOn()	GPIO_SetBits(GPIOC, GPIO_Pin_9)
#define gnd500mVOff()	GPIO_ResetBits(GPIOC, GPIO_Pin_9)

enum { P_OFF = 0, P_1_67, P_ADC_REF, P_3_0, P_EP1};



class SensorM10 : public Sensor
{
public:
	SensorM10(uint8_t num);
	virtual ~SensorM10();

	virtual float read();
};


void powerDa17_16(uint8_t val);
void powerDa12_15(uint8_t val);

#endif /* SENSORM10_H_ */
