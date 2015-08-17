/*
 * configTerem.cpp
 *
 *  Created on: 05 авг. 2015 г.
 *      Author: juvf
 */
#include "configTerem.h"
#include "tasks/Checksum.h"
#include "i2c.h"

#include <string.h>

TDataFlash configTerem;

void intiDefaultConfig()
{
	configTerem.DF_AdapterNum = 200; //дефолтный адресс прибора в протоколе обмена с планшетом
	for(int i=0; i<16; i++)
		configTerem.sensorType[i] = GT_Absent;

	for(int i = 0; i < 8; i++)
	{
		configTerem.DF_CompChan[i] = 0;
		configTerem.a[i][0] = 1.0;
		configTerem.a[i][1] = 1.0;
	}
	configTerem.DF_CRC16 = Checksum::crc16((uint8_t*)&configTerem, sizeof(TDataFlash) - 2);
	i2cWrite(0xa0, 0, (uint8_t*)&configTerem, sizeof(TDataFlash));
}

void initConfigTerem()
{
	i2cRead(0xa0, 0, (uint8_t*)&configTerem, sizeof(TDataFlash));
	if(Checksum::crc16((uint8_t*)&configTerem, sizeof(TDataFlash)) != 0)
		intiDefaultConfig();
}

int setConfigTerem(uint8_t *buffer)
{
	memcpy((void*)&configTerem, (void*)buffer,  sizeof(TDataFlash));
	return 6;
}

int getConfigTerem(uint8_t *buffer)
{
	memcpy((void*)buffer, (void*)&configTerem,  sizeof(TDataFlash));
	return  sizeof(TDataFlash) + 6;
}

