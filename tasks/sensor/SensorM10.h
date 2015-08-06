/*
 * SensorM10.h
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#ifndef SENSORM10_H_
#define SENSORM10_H_

#include "Sensor.h"

class SensorM10 : public Sensor
{
public:
	SensorM10(uint8_t num);
	virtual ~SensorM10();

	virtual float read();
};

#endif /* SENSORM10_H_ */
