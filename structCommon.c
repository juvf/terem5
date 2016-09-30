/*
 * structCommon.c
 *
 *  Created on: 29 февр. 2016 г.
 *      Author: juvf
 */
#include "structCommon.h"

uint32_t u32FromU8(const uint8_t *buffer)
{
	uint32_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	result |= (((uint32_t)buffer[2] & 0xff) << 16);
	result |= (((uint32_t)buffer[3] & 0xff) << 24);
	return result;
}

uint64_t u64FromU8(const uint8_t *buffer)
{
	uint64_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	result |= (((uint32_t)buffer[2] & 0xff) << 16);
	result |= (((uint32_t)buffer[3] & 0xff) << 24);
	result |= (((uint32_t)buffer[4] & 0xff) << 32);
	result |= (((uint32_t)buffer[5] & 0xff) << 40);
	result |= (((uint32_t)buffer[6] & 0xff) << 48);
	result |= (((uint32_t)buffer[7] & 0xff) << 56);
	return result;
}

uint16_t u16FromU8(const uint8_t *buffer)
{
	uint32_t result = buffer[0];
	result |= (((uint32_t)buffer[1] & 0xff) << 8);
	return result;
}
