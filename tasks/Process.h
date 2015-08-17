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

#define MAX_SECTORS	2000

int commandStartProc(uint8_t *buffer);
int commandGetProcConf(uint8_t *buffer);
int commandStopProc(uint8_t *buffer);
int commandGetCountProc(uint8_t *buffer);
int commandGetHeaderProc(uint8_t *buffer);
int commandDeleteProc(uint8_t *buffer);

bool headerIsValid( const HeaderProcess &header);
bool allocMemForNewProc(const HeaderProcess &header);
int getProcessStatus();
void initListProc();
uint32_t calcCountSectors(uint32_t countPoints);
#endif /* TASKS_PROCESS_H_ */
