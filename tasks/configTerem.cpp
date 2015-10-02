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
uint8_t adcRange[8];
KoefAB koefAB;

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
		adcRange[i] = 0;
	}
	configTerem.crc[1] = Checksum::crc16((uint8_t*)&configTerem,
			sizeof(TeremConfig) - 2);
}

void initConfigTerem()
{
	i2cRead(0xa0, 0, (uint8_t*)&configTerem, sizeof(TeremConfig));
	if(Checksum::crc16((uint8_t*)&configTerem, sizeof(TeremConfig)) != 0)
	{
		intiDefaultConfig();
		i2cWrite(0xa0, 0, (uint8_t*)&configTerem, sizeof(TeremConfig));
	}
	for(int i = 0; i < 8; i++)
	{
		adcRange[i] = 0;
	}

	i2cRead(0xa0, sizeof(TeremConfig), (uint8_t*)&koefAB, sizeof(KoefAB));
	if(Checksum::crc16((uint8_t*)&koefAB, sizeof(koefAB)) != 0)
	{
		intiDefaultKoefAB();
		i2cWrite(0xa0, sizeof(TeremConfig), (uint8_t*)&koefAB, sizeof(KoefAB));
	}
	for(int i = 0; i < 8; i++)
	{
		adcRange[i] = 0;
	}
}

	//Запись коэффициентов по умолчанию (0, 1) ------------------------------------
void intiDefaultKoefAB()
{
	unsigned char channel;
	for(int i =0; i<5; i++)
	{
		koefAB.a[i] = 0;
		koefAB.b[i] = 0;
	}
	koefAB.a[1] = 1;
	koefAB.b[1] = 1;

	koefAB.crc[1] = Checksum::crc16((uint8_t*)&koefAB,
				sizeof(KoefAB) - 2);
}

int setConfigTerem(uint8_t *buffer)
{
	buffer += 6;
//	TeremConfig config;
//	memcpy((void*)&config, (void*)buffer,  sizeof(TeremConfig));
//	if(Checksum::crc16((uint8_t*)&config, sizeof(TeremConfig)) == 0)
//		memcpy((void*)&configTerem, (void*)buffer,  sizeof(TeremConfig));

	for(int i = 0; i < 16; i++)//16
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
	configTerem.Flags |= *buffer++ << 8;//2 (30)
	configTerem.DF_AdapterNum = *buffer++;
	configTerem.DF_AdapterNum |= *buffer++ << 8;//2 (32)
	memcpy((void*)&configTerem.a, (void*)(buffer), sizeof(float) * 16);
	buffer += sizeof(float) * 16;// 64 (96)

	configTerem.crc[1] = Checksum::crc16((uint8_t*)&configTerem,
			sizeof(configTerem) - 2);

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
	*buffer++ = (uint8_t)(configTerem.Flags >> 8);//2 (30)
	*buffer++ = (uint8_t)configTerem.DF_AdapterNum;
	*buffer++ = (uint8_t)(configTerem.DF_AdapterNum >> 8);//2 (32)
	memcpy((void*)(buffer), (void*)&configTerem.a, sizeof(float) * 16);
	buffer += sizeof(float) * 16;// 64 (96)

	*buffer++ = (uint8_t)configTerem.crc[0];
	*buffer++ = (uint8_t)(configTerem.crc[0] >> 8);//2 (106)
	*buffer++ = (uint8_t)configTerem.crc[1];
	*buffer++ = (uint8_t)(configTerem.crc[1] >> 8);//2 (108)
	return 100 + 6;
}

