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

extern uint16_t flashMap[2048][2]; //карта памяти флэшь

void initSpi2();
void initDmaSpi2();
void startSpi(uint16_t number);
void spiWait();
void setSpiOut(uint16_t adr, uint8_t data);
void spiWREN();
uint16_t spiRDSR();
void spiSector4kErase(uint16_t numSector);
void spiChipErase();

//менеджер памяти
void getFirstLastProcess(uint32_t *firstHeader, uint32_t *lastFreeHeader);
void findBeginEndFreeMem(uint32_t *beginSector, uint32_t *endSector);


void flashMx25Write(uint8_t *source, uint32_t adrDestination );
void flashMx25Read(void *destination, uint32_t adrSource, uint16_t size );
void flashMx25ReadData(uint8_t *destination, uint32_t adrSource, uint16_t size);
uint16_t countFreeSectors();


uint16_t readStatusRegister();







#endif /* TASKS_FLASHMX25_H_ */
