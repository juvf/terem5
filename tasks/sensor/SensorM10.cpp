/*
 * SensorM10.cpp
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#include "SensorM10.h"

SensorM10::SensorM10(uint8_t num)
	: Sensor(num)
{
}

SensorM10::~SensorM10()
{
}

float SensorM10::read()
{
	return 1.0;
}

