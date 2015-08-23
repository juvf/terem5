/*
 * Process.h
 *
 *  Created on: 05 ���. 2015 �.
 *      Author: juvf
 */

#ifndef TASKS_PROCESS_H_
#define TASKS_PROCESS_H_
#include <stdint.h>
#include "../structCommon.h"

#define MAX_SECTORS	2048

int commandStartProc(uint8_t *buffer);
int commandGetProcConf(uint8_t *buffer);
int commandStopProc(uint8_t *buffer);
int commandGetCountProc(uint8_t *buffer);
int commandGetHeaderProc(uint8_t *buffer);
int commandDeleteProc(uint8_t *buffer);
uint32_t getAdrCurPoint();

bool headerIsValid( const HeaderProcess &header);
bool allocMemForNewProc(const HeaderProcess &header);
int getProcessStatus();
void initListProc();
uint32_t calcCountSectors(const HeaderProcess &header);
#endif /* TASKS_PROCESS_H_ */
