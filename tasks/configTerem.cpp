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

TeremConfig configTerem;

void intiDefaultConfig()
{
	configTerem.DF_AdapterNum = 200; //дефолтный адресс прибора в протоколе обмена с планшетом
	for(int i = 1; i < 16; i++)
		configTerem.sensorType[i] = GT_Absent;
	for(int i = 0; i < 8; i++)
	{
		configTerem.DF_CompChan[i] = 0;
		configTerem.a[i][0] = 1.0;
		configTerem.a[i][1] = 1.0;
		configTerem.adcRange[i] = 0;
	}

	configTerem.a[0][0] = 3.14;
	configTerem.a[0][1] = 0.14;
	configTerem.a[1][0] = 2.14;
	configTerem.a[1][1] = 4.14;
	configTerem.a[3][1] = 31.4;
	int sizeTeremConfig = sizeof(TeremConfig);
	configTerem.DF_CRC16 = Checksum::crc16((uint8_t*)&configTerem, sizeof(TeremConfig) - 4);
}

void initConfigTerem()
{
	i2cRead(0xa0, 0, (uint8_t*)&configTerem, sizeof(TeremConfig));
	if(Checksum::crc16((uint8_t*)&configTerem, sizeof(TeremConfig)) != 0)
	{
		intiDefaultConfig();
		i2cWrite(0xa0, 0, (uint8_t*)&configTerem, sizeof(TeremConfig));
	}
}

int setConfigTerem(uint8_t *buffer)
{
	buffer += 6;
//	TeremConfig config;
//	memcpy((void*)&config, (void*)buffer,  sizeof(TeremConfig));
//	if(Checksum::crc16((uint8_t*)&config, sizeof(TeremConfig)) == 0)
//		memcpy((void*)&configTerem, (void*)buffer,  sizeof(TeremConfig));

	for(int i = 0; i < 16; i++) //16
	{ // uint8_t sensorType[16];     //0x1000, тип датчика в канале
		configTerem.sensorType[i] = *buffer++;
	}

	for(int i = 0; i < 8; i++) //8 (24)
	{ // unsigned char DF_CompChan[8]; //Номер канала компенсатора для датчика канала 0 (термопары, датчика влажности)
		configTerem.DF_CompChan[i] = *buffer++;
	}

	memcpy((void*)&configTerem.Vref, (void*)(buffer), sizeof(float));
	buffer += sizeof(float); //4 (28)

	configTerem.Flags = *buffer++;
	configTerem.Flags |= *buffer++ << 8; //2 (30)
	configTerem.DF_AdapterNum = *buffer++;
	configTerem.DF_AdapterNum |= *buffer++ << 8; //2 (32)
	memcpy((void*)&configTerem.a, (void*)(buffer), sizeof(float) * 16);
	buffer += sizeof(float) * 16; // 64 (96)

	for(int i = 0; i < 8; i++)
	{ // uint8_t adcRange[8]; //8 (104)
		configTerem.adcRange[i] = *buffer++;
	}
	configTerem.DF_CRC16 = Checksum::crc16( (uint8_t*)&configTerem, sizeof(configTerem) - 2 );

	i2cWrite(0xa0, 0, (uint8_t*)&configTerem, sizeof(TeremConfig));

	return 6;
}

int getConfigTerem(uint8_t *buffer)
{
	buffer += 6;
	for(int i = 0; i < 16; i++) //16
	{ // uint8_t sensorType[16];     //0x1000, тип датчика в канале
		*buffer++ = configTerem.sensorType[i];
	}
	for(int i = 0; i < 8; i++) //8 (24)
	{ // unsigned char DF_CompChan[8]; //Номер канала компенсатора для датчика канала 0 (термопары, датчика влажности)
		*buffer++ = configTerem.DF_CompChan[i];
	}

	memcpy((void*)(buffer), (void*)&configTerem.Vref, sizeof(float));
	buffer += sizeof(float); //4 (28)
	*buffer++ = (uint8_t)configTerem.Flags;
	*buffer++ = (uint8_t)(configTerem.Flags >> 8); //2 (30)
	*buffer++ = (uint8_t)configTerem.DF_AdapterNum;
	*buffer++ = (uint8_t)(configTerem.DF_AdapterNum >> 8); //2 (32)
	memcpy((void*)(buffer), (void*)&configTerem.a, sizeof(float) * 16);
	buffer += sizeof(float) * 16; // 64 (96)

	for(int i = 0; i < 8; i++)
	{ // uint8_t adcRange[8]; //8 (104)
		*buffer++ = configTerem.adcRange[i];
	}
	*buffer++ = (uint8_t)configTerem.DF_CRC16;
	*buffer++ = (uint8_t)(configTerem.DF_CRC16 >> 8); //2 (106)
	return 106 + 6;
}

