/*
 * commandsRfd.cpp
 *
 *  Created on: 16 ��� 2015 �.
 *      Author: juvf
 */
#include "commandsRfd.h"
#include "clock.h"
int commandTestConnect(uint8_t *buffer)
{
	return 10;
}

int commandError(uint8_t *buffer)
{
	buffer[5] = 0x0E;
	return 6;
}

//������������ ������� �
int commandSetConfig(uint8_t *buffer)
{
	return 10;
}

int commandGetConfig(uint8_t *buffer)
{
	return 10;
}


