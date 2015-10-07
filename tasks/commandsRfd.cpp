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
#include "Process.h"
#include "../osConfig.h"
#include "configTerem.h"
#include "sensor/Sensor.h"


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
		countProc = 0;
		spiChipErase();
	}
	return 6;
}

int commandGetCurAdc(uint8_t *buffer)
{
	//захватим симафор АЦП
	if(buffer[6] > 7)
	{
		buffer[5] = 0x0e;
		return 6;
	}
	else
	{
		int numChanel = buffer[6];
		xSemaphoreTake(semaphAdc, portMAX_DELAY);
		ep1_On();
		epa_On();
		ResultMes result = readSenser(numChanel);
//		float valU;
//		switch(configTerem.sensorType[numChanel])
//		{
//			//Датчики перемещения
//			case GT_MM10:
//				valU = readAnalogSensor(numChanel);
//				valP = MM10_Length(valU, configTerem.a[numChanel][0]);
////				valP = valU * 2.0 / 1.17;
////				valP = (valP - configTerem.a[buffer[6]][0]) * 5.5; //Результат в мм
//				break;
//			case GT_MM20:
//				valU = readAnalogSensor(numChanel);
//				valP = MM20_Length(valU, configTerem.a[numChanel][0]);
//				break;
//			case GT_MM50:
//				valU = readAnalogSensor(numChanel);
//				valP = MM50_Length(valU, configTerem.a[numChanel][0]);
//				break;
//			case GT_HEL700:
//				valU = getU_Ad7792(numChanel);
//				valP = HEL700_Termo(valU, numChanel);
//				break;
//			default:
//				valP = 0;
//				break;
//		}
		epa_Off();
		ep1_Off();
		//освободим симафор АЦП
		xSemaphoreGive(semaphAdc);
		memcpy((void*)&buffer[7], (void*)&result.u, 4);
		memcpy((void*)&buffer[11], (void*)&result.p, 4);
		return 15;
	}
}

int commandReadFlash(uint8_t *buffer)
{
	uint32_t adrInFlash = buffer[6] | (buffer[7] << 8) | (buffer[8] << 16);
	uint16_t size = buffer[9];
	if((size > 248) || (adrInFlash > (8 * 1024 * 1024 - size)))
	{
		buffer[11] = 0x0E;
		return 6;
	}
	flashMx25ReadData(&buffer[6], adrInFlash, size);
	for(int i = 0; i < size; i++)
		buffer[6 + i] = buffer[6 + i + 4];
//	memcpy((void*)&buffer[6], (void*)&buffer[10], size-4);
	return 6 + size;
}

int commandT48(uint8_t *buffer)
{
	if((buffer[7] > 7) || (buffer[6] > 1))
		return commandError(buffer);
	if(buffer[6] == 0)
	{ //чтение
		int numChanel = buffer[7];
		buffer[6] = configTerem.sensorType[numChanel];
		memcpy((void*)&buffer[7], (void*)&koeffsAB.koef[numChanel],
				10 * sizeof(float));
		return 40 + 7;
	}
	else
	{ //запись
		configTerem.sensorType[buffer[7]] = buffer[8];
		saveConfig();
		memcpy((void*)&koeffsAB.koef[buffer[7]], (void*)&buffer[9],
				10 * sizeof(float));
		saveKoeffAB();
		return 6;
	}
}

