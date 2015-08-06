/*
 * Sensor.cpp
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#include "Sensor.h"
#include "../../structCommon.h"

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
	ret
}

void Sensor::setType(uint8_t type_)
{
	type = type_;
}

bool Sensor::isEnabled() const
{

}

