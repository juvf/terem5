/*
 * Dpg2.h
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_DPG2_H_
#define TASKS_SENSOR_DPG2_H_

#include <stdint.h>

float dpg2_readValue(uint8_t numChanel);
void init_Dtg2();
uint32_t readDtg();

#endif /* TASKS_SENSOR_DPG2_H_ */
