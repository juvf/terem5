/*
 * Dpg2.cpp
 *
 *  Created on: 26 ����. 2016 �.
 *      Author: anisimov.e
 */

#include "Dpg2.h"
#include "Sensor.h"

float dpg2_readValue()
{
	//������� �����
	gnd500mVOff();
	//������� �������
	powerDa17_16(P_3_0);
	powerDa12_15(0);

	return 1.0;
}

