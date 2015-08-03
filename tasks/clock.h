/*
 * clock.h
 *
 *  Created on: 25 июля 2015 г.
 *      Author: juvf
 */

#ifndef TASKS_CLOCK_H_
#define TASKS_CLOCK_H_

#include <stdint.h>

void initRtc();
bool setRtcTime(uint8_t *buffer);
void getRtcTime(uint8_t *buffer);



#endif /* TASKS_CLOCK_H_ */
