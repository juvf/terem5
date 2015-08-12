/*
 * flashMx25.h
 *
 *  Created on: 07 рту. 2015 у.
 *      Author: anisimov.e
 */

#ifndef TASKS_FLASHMX25_H_
#define TASKS_FLASHMX25_H_

#include <stdint.h>
#include "structCommon.h"
#include "stm32f4xx.h"

#define csOn()	GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define csOff()	GPIO_SetBits(GPIOC, GPIO_Pin_5)

void initSpi2();
void initDmaSpi2();
void startSpi(uint16_t number);
void spiWait();
void setSpiOut(uint16_t adr, uint8_t data);
void spiWREN();
uint16_t spiRDSR();
void spiSector4kErase(uint16_t numSector);


void saveHeader(HeaderProcess* header);
void savePoint(float *aray, uint8_t size);
void closeProcess(HeaderProcess* header);
void flashMx25Write(uint8_t *source, uint32_t adrDestination );
void flashMx25Read(uint8_t *destination, uint32_t adrSource );


uint16_t readStatusRegister();





#endif /* TASKS_FLASHMX25_H_ */
