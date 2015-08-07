/*
 * taskMeasurement.h
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: juvf
 */

#ifndef TASKS_TASKMEASUREMENT_H_
#define TASKS_TASKMEASUREMENT_H_
#include "osConfig.h"

void musuring();

#ifdef __cplusplus
extern "C"
{
#endif

void taskMeasurement(xTimerHandle xTimer);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_TASKMEASUREMENT_H_ */
