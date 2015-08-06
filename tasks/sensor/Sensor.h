/*
 * Sensor.h
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_H_
#define TASKS_SENSOR_H_

#include <stdint.h>

class Sensor
{
public:
	Sensor(uint8_t num);
	virtual ~Sensor();

	virtual float read() = 0;
	uint8_t getType() const;
	bool isEnabled() const;

protected:
	uint8_t numChanel;
	uint8_t type;
};

#endif /* TASKS_SENSOR_H_ */
