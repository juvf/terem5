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
#include "sensor/ds1820.h"
#include "adc.h"
#include "sensor/Dpg2.h"

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

int commandGetState(uint8_t *buffer)
{
	buffer[6] = getBatValue();
	buffer[7] = (stateProcess == 1) || (stateProcess == 3) ? 1 : 0;
	return 8;
}

int commandClearFlash(uint8_t *buffer)
{
	uint8_t state = getProcessStatus();
	if( (state == 1) || (state == 3) )
		*buffer = 0x0E;
	else
	{
		EventBits_t flags = xEventGroupGetBits(xEventGroup);
		if( flags & FLAG_FLASH_CLEARING )
		{
			*buffer++ = 0x0d; //устройство занято
			*buffer = 0x04; //устройство зянято стиранием памяти
			return 7;
		}
		else
		{
			memset((void*)flashMap, 0xff, sizeof(flashMap));
			memset((void*)headerList, 0xff, sizeof(headerList));
			countProc = 0;
			spiChipErase();
			xEventGroupSetBits(xEventGroup, FLAG_FLASH_CLEARING);
			//стартуем таймер на 22 сек, после которого state сменим с 4 на 2.
			xTimerStart(timerClearFlash, 500);
		}
	}
	return 6;
}

int commandReadyCheck(uint8_t *buffer)
{
	buffer[5] = 0x0d;
	if( xSemaphoreTake(semaphAdc, 0) == pdTRUE )
	{
		EventBits_t uxBits = xEventGroupGetBits(xEventGroup);
		if( (uxBits & FLAG_IS_READY_MES) == FLAG_IS_READY_MES )
			buffer[6] = 3; //Устройство не занято, есть данные
		else
			buffer[6] = 2; //Устройство не занято, нет данных или нет процесса
		xSemaphoreGive(semaphAdc);
	}
	else
	{
		buffer[6] = 1;
	}
	return 7;
}

int commandGetCurAdc(uint8_t *buffer)
{
	int numChanel = buffer[6];
	if( (numChanel > 8) && (numChanel != 57) )
	{ //недопустимый адресс
		buffer[5] = 0x0e;
		return 6;
	}
	else
	{
		//захватим симафор АЦП
		xSemaphoreTake(semaphAdc, portMAX_DELAY);
		if( configTerem.sensorType[numChanel] != GT_SHT21 )
			tempOfDs1820 = readtemp();
		//ep1_On();
		//epa_On();

		uint16_t curN;
		ResultMes result;
		if( numChanel < 9 )
		{
			tempOfDs1820 += configTerem.deltaT;
			result = readSenser(numChanel, &curN);
		}
		else
			result.p = tempOfDs1820;
		//epa_Off();
		//ep1_Off();
		//освободим симафор АЦП
		xSemaphoreGive(semaphAdc);

		if( configTerem.sensorType[numChanel] == GT_SHT21 )
		{
			memcpy((void*)&buffer[7], (void*)&result.uClear, 4);
			memcpy((void*)&buffer[11], (void*)&result.p, 4);
			memcpy((void*)&buffer[15], (void*)&result.u, 4);
			return 19;
		}
		else
		{
			memcpy((void*)&buffer[7], (void*)&result.u, 4);
			memcpy((void*)&buffer[11], (void*)&result.p, 4);
			memcpy((void*)&buffer[15], (void*)&curN, 2);
			memcpy((void*)&buffer[18], (void*)&result.uClear, 4);
		}

		switch(numChanel)
		{
			case 57:
				buffer[17] = GT_TermoHK;
				break;
			default:
				buffer[17] = configTerem.sensorType[numChanel];
		}
		return 22;
	}
}

int commandReadFlash(uint8_t *buffer)
{
	uint8_t ttBuf[200];
	uint32_t adrInFlash = buffer[6] | (buffer[7] << 8) | (buffer[8] << 16);
	uint16_t size = buffer[9];
	if( (size > 248) || (adrInFlash > (8 * 1024 * 1024 - size)) )
	{
		buffer[11] = 0x0E;
		return 6;
	}
//	flashMx25ReadData(&buffer[6], adrInFlash, size);
	flashMx25ReadData(ttBuf, adrInFlash, size);
	memcpy((void*)&buffer[6], (void*)ttBuf, 200);
	for(int i = 0; i < size; i++)
		buffer[6 + i] = buffer[6 + i + 4];
	return 6 + size;
}

int commandT48(uint8_t *buffer)
{
	if( (buffer[7] > 7) || (buffer[6] > 1) )
		return commandError(buffer);
	if( buffer[6] == 0 )
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

int commandReadVersion(uint8_t *buffer)
{
	char *p = __DATE__;
	int size = strlen(p);
	memcpy((void*)&buffer[6], (void*)p, size);
	return 6 + size;
}

