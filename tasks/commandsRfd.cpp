/*
 * commandsRfd.cpp
 *
 *  Created on: 16 ��� 2015 �.
 *      Author: juvf
 */
#include "commandsRfd.h"
#include "clock.h"
#include "Process.h"
#include "flashMx25.h"
#include "Process.h"

#include <string.h>

int commandTestConnect(uint8_t *buffer)
{
	return 10;
}

int commandError(uint8_t *buffer)
{
	buffer[5] = 0x0E;
	return 6;
}

int commandClearFlash(uint8_t *buffer)
{
	uint8_t state = getProcessStatus();
	if((state == 1) || (state == 3))
		*buffer = 0x0E;
	else
	{
		memset((void*)flashMap, 0xff, sizeof(flashMap));
		memset((void*)headerList, 0xff, sizeof(headerList));
		spiChipErase();
	}
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

