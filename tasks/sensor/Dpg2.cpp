/*
 * Dpg2.cpp
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: anisimov.e
 */

#include "Dpg2.h"
#include "Sensor.h"

float dpg2_readValue()
{
	//подадим землю
	gnd500mVOff();
	//подадим питание
	powerDa17_16(P_3_0);
	powerDa12_15(0);

	return 1.0;
}

