/*
 * Sensor.cpp
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#include "Sensor.h"
#include "../../structCommon.h"

uint8_t typeOfSensor[9] = { GT_Absent };

float readSenser(uint8_t numChanel)
{
	float value;
	switch(typeOfSensor[numChanel])
	{
		case GT_MM10:
			value = 10.1;
			break;
		case GT_MM20:
			value = 20.2;
			break;
		case GT_MM50:
			value = 50.2;
			break;
		default:
			value = 0;
			break;
	}
	return value;
}

Sensor::Sensor(uint8_t num)
{
	numChanel = num;
	type = GT_Absent;
}

Sensor::~Sensor()
{
}

uint8_t Sensor::getType() const
{
	return type;
}

void Sensor::setType(uint8_t type_)
{
	type = type_;
}

bool Sensor::isEnabled() const
{

}

