/*
 * clock.h
 *
 *  Created on: 25 июля 2015 г.
 *      Author: juvf
 */

#ifndef TASKS_CLOCK_H_
#define TASKS_CLOCK_H_

#include <stdint.h>

#include "stm32f4xx_rtc.h"

void initRtc();
bool setRtcTime(uint8_t *buffer);
void getRtcTime(uint8_t *buffer);
void setNewAlarmRTC(uint32_t seconds);
bool addSecToTime(RTC_TimeTypeDef *time, uint32_t seconds);


#endif /* TASKS_CLOCK_H_ */
