/*
 * Process.cpp
 *
 *  Created on: 05 авг. 2015 г.
 *      Author: juvf
 */
#include "Process.h"
#include "stm32f4xx_rtc.h"
#include "../osConfig.h"
#include "taskMeasurement.h"
#include "Checksum.h"
#include "../flashMx25.h"
#include  <string.h>

uint8_t stateProcess; //текущее состо€ние процесса 0-нет процесса, 1-едЄт процесс, 2-закончилс€, 3-ждет старта
HeaderProcess currProcessHeader; //заголовок текущего процесса
uint32_t currProcessCount; //кол-во записанных точек

uint16_t headerList[MAX_SECTORS] = { 0xffff }; //
uint16_t countProc = 0; //кол-во процессов в флешке

struct Header
{
	uint16_t preNext[2];
	HeaderProcess header;
};

//сканирование флешки и заполненеи массива указателей заголовков процесса headerList[]
void initListProc()
{
	countProc = 0;
	struct Header
	{
		uint16_t preNext[2];
		HeaderProcess header;
	} header;
	for(int i = 0; i < MAX_SECTORS; i++)
	{
		headerList[i] = 0xffff;
		flashMx25Read((void*)&header, i * 4096, sizeof(Header));
		if(headerIsValid(header.header) && (header.preNext[0] != 0xffff))
		{
			headerList[countProc++] = i;
		}
	}
}

int commandGetCountProc(uint8_t *buffer)
{
	buffer[6] = countProc;
	buffer[7] = countProc >> 8;
	return 8;
}

int commandGetHeaderProc(uint8_t *buffer)
{
	uint16_t number = buffer[6];
	number = buffer[7] >> 8;

	if(number > MAX_SECTORS)
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x01;
		return 7;
	}
	uint32_t addrInFlash = headerList[number];

	if(addrInFlash == 0xffff)
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x04;
		return 7;
	}
	Header header;
	flashMx25Read((void*)&header, addrInFlash, sizeof(Header));
	if(!headerIsValid(header.header))
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x02;
		return 7;
	}
	memcpy(buffer + 6, (void*)&header.header, sizeof(HeaderProcess));
	uint32_t countSectords = calcCountSectors(header.header.count);

	//найдем цепочку секторов
	buffer += (size_t)buffer + sizeof(HeaderProcess) + 6;
	buffer[0] = header.preNext[0];
	buffer[1] = header.preNext[0] >> 8;
	if(countSectords > 1)
	{
		buffer[2] = header.preNext[1];
		buffer[3] = header.preNext[1] >> 8;
	}
	if(countSectords > 2)
	{
		for(int i = 2; i < countSectords; i++)
		{
			flashMx25Read((void*)&header, header.preNext[1], 4);
			if(header.preNext[1] == 0xffff)
				break;
			buffer[i * 2] = header.preNext[1];
			buffer[i * 2 + 1] = header.preNext[1] >> 8;
		}
	}

	return 6 + countSectords * 2 + sizeof(HeaderProcess);
}

uint32_t calcCountSectors(uint32_t countPoints)
{
	uint32_t sizeOfProcessData = 4 * countPoints;
//рассчитаем кол-во секторов
	uint32_t remainder = sizeOfProcessData % 4096;
	uint32_t countSectords = 1 + (sizeOfProcessData / 4096);
	uint16_t sizeOfHeader = sizeof(Header) + 4 + (countSectords * 2); //зармер заголовка и цепочки адресов секторов
	if((4096 - sizeOfHeader) < remainder)
		countSectords++;
	return countSectords;
}

int commandDeleteProc(uint8_t *buffer)
{
	uint16_t number = buffer[6];
	number = buffer[7] >> 8;
	if(number > MAX_SECTORS)
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x01;
		return 7;
	}

	if(headerList[number] == 0xffff)
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x04;
		return 7;
	}

	Header header;

	flashMx25Read((void*)&header, headerList[number], sizeof(Header));
	if(!headerIsValid(header.header))
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x02;
		return 7;
	}

	uint32_t countSectords = calcCountSectors(header.header.count);
	//сотрЄм цепочку секторов
	spiSector4kErase(headerList[number]);
	if(countSectords > 1)
	{
		for(int i = 1; i < countSectords; i++)
		{
			uint32_t adrInFlash = header.preNext[1];
			flashMx25Read((void*)&header, adrInFlash, 4);
			spiSector4kErase(adrInFlash);
			if(header.preNext[1] == 0xffff)
				break;
		}
	}
	//сдвиним весь массив headerList
	for(int i = number+1; i<MAX_SECTORS; i++)
	{
		headerList[number-1] = headerList[number];
		if(headerList[number] == 0xffff)
			break;
	}
	headerList[MAX_SECTORS-1] = 0xffff;
	return 6;
}

int getProcessStatus()
{
	return stateProcess;
}

int commandStartProc(uint8_t *buffer)
{
	if((stateProcess == 0) || (stateProcess == 2))
	{
		uint32_t per = (buffer[9] << 24) | (buffer[8] << 16) | (buffer[7] << 8)
				| buffer[6];
		if((per > 0) && (per < 4294960))
		{
			currProcessHeader.period = per;
			if(buffer[0] == 0xff)
			{ //старотовать сейчас процесс
			  //заполнить заголовок процесса
				if(allocMemForNewProc(currProcessHeader))
				{
					musuring();
					if( xTimerChangePeriod(timerMesuring, per * 1000,
							100) == pdFAIL)
						buffer[0] = 1;
					else
					{
						xTimerReset(timerMesuring, 100);
						buffer[0] = 0;
					}
				}
				else
					buffer[0] = 2;
			}
			else
			{
				currProcessHeader.startTime.RTC_Seconds = buffer[0];
				currProcessHeader.startTime.RTC_Minutes = buffer[1];
				currProcessHeader.startTime.RTC_Hours = buffer[2];
				currProcessHeader.startDate.RTC_Date = buffer[3];
				currProcessHeader.startDate.RTC_Month = buffer[4];
				currProcessHeader.startDate.RTC_Year = buffer[5];
				buffer[0] = 0;
			}
		}
		else
			buffer[0] = 1;
	}
	else
	{ //процес или идЄт, или ждЄт старта, поэтому нельз€ мен€ть параметры процесса
		buffer[0] = 1;
	}
	return 7;
}

int commandGetProcConf(uint8_t *buffer)
{
	buffer[0] = currProcessHeader.startTime.RTC_Seconds;
	buffer[1] = currProcessHeader.startTime.RTC_Minutes;
	buffer[2] = currProcessHeader.startTime.RTC_Hours;
	buffer[3] = currProcessHeader.startDate.RTC_Date;
	buffer[4] = currProcessHeader.startDate.RTC_Month;
	buffer[5] = currProcessHeader.startDate.RTC_Year;
	buffer[6] = currProcessHeader.period & 0xff;
	buffer[7] = currProcessHeader.period >> 8;
	return 14;
}

int commandStopProc(uint8_t *buffer)
{
	return 0;
}

bool headerIsValid(const HeaderProcess &header)
{
	return (Checksum::crc16((uint8_t*)&header, sizeof(HeaderProcess)) == 0);
}

bool allocMemForNewProc(const HeaderProcess &header)
{
//находим цепочку секторов
//рассчитаем кол-во необходимых секторов
	uint8_t countSensor = 0;
	for(int i = 0; i < 16; i++)
	{
		if(header.config.sensorType[i] < GT_Absent)
			countSensor++;
	}

	uint32_t dataSize = countSensor * sizeof(float) * header.count;	//размер данных процесса в байтах

	for(int i = 0; i < 2048; i++)
	{

	}
//записываем заголовок и цепочку секторов
//метим сектора дл€ данных
//если нет места дл€ процесса, везвращ€ем false
	return false;
}
