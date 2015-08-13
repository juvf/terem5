/*
 * Process.h
 *
 *  Created on: 05 рту. 2015 у.
 *      Author: juvf
 */

#ifndef TASKS_PROCESS_H_
#define TASKS_PROCESS_H_
#include <stdint.h>
#include "../structCommon.h"

int commandStartProc(uint8_t *buffer);
int commandGetProcConf(uint8_t *buffer);
int commandStopProc(uint8_t *buffer);

bool headerIsValid( const HeaderProcess &header);

#endif /* TASKS_PROCESS_H_ */
