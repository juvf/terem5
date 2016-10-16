/*
 * flashMx25.h
 *
 *  Created on: 07 авг. 2015 г.
 *      Author: anisimov.e
 */

#ifndef TASKS_FLASHMX25_H_
#define TASKS_FLASHMX25_H_

#include <stdint.h>
#include "structCommon.h"
#include "stm32f4xx.h"

#ifdef __cplusplus

void flashMx25ReadData(uint8_t *destination,
		uint32_t adrSource, uint16_t size, bool fromISR = false);
extern "C"
{
#endif


extern uint16_t flashMap[2048][2]; //карта памяти флэшь

void initSpi2();
void deinitSpi2();
void spiSector4kErase(uint32_t numSector);
void spiChipErase();


void flashMx25Write(uint8_t *source, uint32_t adrDestination );
void flashMx25Read(void *destination, uint32_t adrSource, uint16_t size );

uint16_t countFreeSectors();

#ifdef __cplusplus
}
#endif







#endif /* TASKS_FLASHMX25_H_ */
