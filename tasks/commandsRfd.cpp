/*
 * commandsRfd.cpp
 *
 *  Created on: 16 мая 2015 г.
 *      Author: juvf
 */
#include "commandsRfd.h"
#include "clock.h"
#include "Process.h"
#include "flashMx25.h"

int commandTestConnect(uint8_t *buffer)
{
	return 10;
}

int commandError(uint8_t *buffer)
{
	buffer[5] = 0x0E;
	return 6;
}

//конфигурация пишется в
int commandSetConfig(uint8_t *buffer)
{
	return 10;
}

int commandGetConfig(uint8_t *buffer)
{
	return 10;
}

int commandClearFlash(uint8_t *buffer)
{
	uint8_t state = getProcessStatus();
	if((state == 1) || (state == 3))
		*buffer = 0x0E;
	return 6;
}

int commandReadFlash(uint8_t *buffer)
{
	uint32_t adrInFlash = buffer[6] | (buffer[7] << 8) | (buffer[8] << 16);
	uint16_t size = buffer[9] | (buffer[10] << 8);
	if((size >= 4096) || (adrInFlash > (8 * 1024 * 1024 - size)))
	{
		buffer[11] = 0x0E;
		return 6;
	}
	flashMx25ReadData(&buffer[6], adrInFlash, size);
	return 6 + size;
}

