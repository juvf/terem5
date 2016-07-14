/*
 * Process.cpp
 *
 *  Created on: 05 авг. 2015 г.
 *      Author: juvf
 */
#include "Process.h"
#include "clock.h"
#include "stm32f4xx_rtc.h"
#include "../osConfig.h"
#include "taskMeasurement.h"
#include "Checksum.h"
#include "../flashMx25.h"
#include "configTerem.h"
#include "osConfig.h"
//#include "../structCommon.h"

#include  <string.h>

uint16_t numProc = 0xffff; //номер текущего процесса в headerList[]
HeaderProcess currProcessHeader; //заголовок текущего процесса
uint32_t currProcessCount; //кол-во записанных точек
uint8_t stateProcess; //текущее состояние процесса 0-нет процесса, 1-идёт процесс, 2-закончился, 3-ждет старта

uint16_t headerList[MAX_SECTORS];
//
uint16_t countProc = 0; //кол-во процессов в флешке, включая текущий

struct Header
{
	uint16_t preNext[2];
	HeaderProcess header;
};

//uint32_t getAdrCurPoint()
//{
//
//}

uint32_t u32FromU8(uint8_t *buf)
{
	uint32_t r = (uint32_t)buf[0] & 0xff;
	r |= ((uint32_t)buf[1] & 0xff) << 8;
	r |= ((uint32_t)buf[2] & 0xff) << 16;
	r |= ((uint32_t)buf[3] & 0xff) << 24;
	return r;
}

//сканирование флешки и заполненеи массива указателей заголовков процесса headerList[]
void initListProc()
{
	memset((void*)flashMap, 0xff, sizeof(flashMap));
	memset((void*)headerList, 0xff, sizeof(headerList));
	countProc = 0;
	Header header;
	for(int i = 0; i < MAX_SECTORS; i++)
	{
		headerList[i] = 0xffff;
		flashMx25Read((void*)&header, i * 4096, sizeof(Header));
		flashMap[i][0] = header.preNext[0];
		flashMap[i][1] = header.preNext[1];
		if( headerIsValid(header.header) && (header.preNext[0] != 0xffff) )
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
	number += buffer[7] >> 8;

	if( number > MAX_SECTORS )
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x01;
		return 7;
	}
	uint32_t addrInFlash = headerList[number] * 4096;

	if( headerList[number] == 0xffff )
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x04;
		return 7;
	}
	Header header;
	flashMx25Read((void*)&header, addrInFlash, sizeof(Header));
	if( !headerIsValid(header.header) )
	{
		buffer[5] = 0x0e;
		buffer[6] = 0x02;
		return 7;
	}
	memcpy(buffer + 6, (void*)&header.header, sizeof(HeaderProcess));
	buffer += 6 + sizeof(HeaderProcess);
	*buffer++ = addrInFlash;
	*buffer++ = addrInFlash >> 8;
	*buffer++ = addrInFlash >> 16;
	*buffer++ = addrInFlash >> 24;

	return 6 + /*countSectords * 2*/+sizeof(HeaderProcess) + 4;
}

//uint32_t calcCountSectors(const HeaderProcess &header)
//{
//	uint16_t countSenser = countSensor(header);
//	uint32_t sizeOfProcessData = 4 * header.count * countSenser;
////рассчитаем кол-во секторов
//	uint32_t remainder = sizeOfProcessData % 4096;
//	uint32_t countSectords = 1 + (sizeOfProcessData / 4096);
//	uint16_t sizeOfHeader = sizeof(Header) + 4 + (countSectords * 2); //зармер заголовка и цепочки адресов секторов
//	if( (4096 - sizeOfHeader) < remainder )
//		countSectords++;
//	return countSectords;
//}
uint32_t calcCountSectors(const HeaderProcess &header)
{
	uint32_t headerSize = sizeof(HeaderProcess);
	uint16_t countSenser = countSensor(header);
	uint32_t allDataSize = header.count * countSenser * sizeof(float);
	allDataSize += headerSize;
	uint16_t sec = allDataSize / 4092;
	uint32_t remainder = allDataSize % 4092;
	sec += (remainder > 0) ? 1 : 0;
	allDataSize += sec * 2;
	uint16_t newSec = allDataSize / 4092;
	newSec += allDataSize % 4092 > 0 ? 1 : 0;
	if( newSec > sec )
	{
		allDataSize += 2;
		sec = allDataSize / 4092;
		sec += allDataSize % 4092 > 0 ? 1 : 0;
	}
	return sec;
}

/* находит в headerList[] процесс с адресом заголовка address и возвращяет индекс массива headerList[]
 * если нет такого процесса, возвращает -1
 */
int getNumProcFromHeaderAdr(uint32_t address)
{
	for(int i = 0; i < MAX_SECTORS; i++)
	{
		if( address == (headerList[i] * 4096) )
			return i;
	}
	return -1;
}

int commandDeleteProc(uint8_t *buffer)
{
	uint32_t headerAddress = u32FromU8(&buffer[6]);
	if( ((headerAddress % 4096) != 0)
			|| (headerAddress > ((MAX_SECTORS - 1) * 4096)) )
	{
		buffer[6] = 1; //недопустимый адресс процесса в запросе
		return 7;
	}

	int number = getNumProcFromHeaderAdr(headerAddress);
	if( number < 0 )
	{
		buffer[6] = 4; //нет процесса, с таким адресом
		return 7;
	}
	Header header;
	flashMx25Read((void*)&header, headerAddress, sizeof(Header));
	if( !headerIsValid(header.header) )
	{
		buffer[6] = 0x02; //ошибка заголовка процесса во флеше
		return 7;
	}

	uint32_t countSectords = calcCountSectors(header.header);
	//сотрём цепочку секторов
	spiSector4kErase(headerList[number] * 4096);
	if( countSectords > 1 )
	{
		for(int i = 1; i < countSectords; i++)
		{
			uint32_t adrInFlash = header.preNext[1] * 4096;
			flashMx25Read((void*)&header, adrInFlash, 4);
			spiSector4kErase(adrInFlash);
			if( header.preNext[1] == 0xffff )
				break;
		}
	}
	//сдвиним весь массив headerList
	for(int i = number + 1; i < MAX_SECTORS; i++)
	{
		headerList[i - 1] = headerList[i];
		if( headerList[i] == 0xffff )
			break;
	}
	headerList[MAX_SECTORS - 1] = 0xffff;
	buffer[6] = 0;
	countProc--;
	return 7;
}

int getProcessStatus()
{
	return stateProcess;
}

int commandStartProc(uint8_t *buffer)
{
	if( (stateProcess == 0) || (stateProcess == 2) )
	{
		uint32_t per = (buffer[9] << 24) | (buffer[8] << 16) | (buffer[7] << 8)
				| buffer[6];
		if( (per > 0) && (per < 4294960) )
		{
			currProcessHeader.period = per;
			currProcessHeader.countSaved = 0xffffffff;
			currProcessHeader.count = buffer[10] | (buffer[11] << 8)
					| (buffer[12] << 16) | (buffer[13] << 24);
			if( buffer[0] == 0xff )
			{ //старотовать сейчас процесс

				RTC_TimeTypeDef time;
				RTC_GetTime(RTC_Format_BIN, &time);
				RTC_DateTypeDef date;
				RTC_GetDate(RTC_Format_BIN, &date);

				currProcessHeader.startTime.RTC_Seconds = time.RTC_Seconds;
				currProcessHeader.startTime.RTC_Minutes = time.RTC_Minutes;
				currProcessHeader.startTime.RTC_Hours = time.RTC_Hours;
				currProcessHeader.startDate.RTC_Date = date.RTC_Date;
				currProcessHeader.startDate.RTC_Month = date.RTC_Month;
				currProcessHeader.startDate.RTC_Year = date.RTC_Year;
				currProcessHeader.config = configTerem;

				//заполнить заголовок процесса
				if( allocMemForNewProc(currProcessHeader) )
				{
					currProcessCount = 0; //кол-во записанных точек в процессе
					//musuring();
					xEventGroupSetBits(xEventGroup, FLAG_MESUR);
					setNewAlarmRTC(currProcessHeader.period);
//					setNewAlarmRTC(4);
//
//					if( xTimerChangePeriod(timerMesuring, per * 1000,
//							100) == pdFAIL)
//						buffer[0] = 4;
//					else
//					{
//						xTimerReset(timerMesuring, 100);
//						buffer[0] = 0;
//						stateProcess = 1;
//					}
					buffer[0] = 0;
					stateProcess = 1;
				}
				else
					buffer[0] = 3;
			}
			else
			{
				currProcessHeader.startTime.RTC_Seconds = buffer[0];
				currProcessHeader.startTime.RTC_Minutes = buffer[1];
				currProcessHeader.startTime.RTC_Hours = buffer[2];
				currProcessHeader.startDate.RTC_Date = buffer[3];
				currProcessHeader.startDate.RTC_Month = buffer[4];
				currProcessHeader.startDate.RTC_Year = buffer[5];
				buffer[0] = 2;
			}
		}
		else
			buffer[0] = 1;
	}
	else
	{ //процес или идёт, или ждёт старта, поэтому нельзя менять параметры процесса
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
	buffer[8] = currProcessHeader.period >> 16;
	buffer[9] = currProcessHeader.period >> 24;
	buffer[10] = currProcessHeader.count & 0xff;
	buffer[11] = currProcessHeader.count >> 8;
	buffer[12] = currProcessHeader.count >> 16;
	buffer[13] = currProcessHeader.count >> 24;
	buffer[14] = currProcessCount & 0xff;
	buffer[15] = currProcessCount >> 8;
	buffer[16] = currProcessCount >> 16;
	buffer[17] = currProcessCount >> 24;
	buffer[18] = stateProcess;

	return 19 + 6;
}

int commandStopProc()
{
	if( (stateProcess == 1) || (stateProcess == 3) )
	{
		taskENTER_CRITICAL();
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		taskEXIT_CRITICAL();
		closeProc();
		stateProcess = 2;
	}
	return 6;
}

void closeProc()
{
	//запишим кол-во сохраненых точек в заголовок процесса
	uint16_t sector = headerList[numProc]; //номер сектора с заголовком процесса
	uint8_t tempBuf[256];
	memset((void*)tempBuf, 0xff, 256);
	memcpy((void*)&tempBuf[4], (void*)&currProcessCount, 4);
	flashMx25Write(tempBuf, sector * 4096);
}

bool headerIsValid(const HeaderProcess &header)
{
	return (Checksum::crc16((uint8_t*)&header.config, sizeof(TeremConfig)) == 0);
}

bool allocMemForNewProc(const HeaderProcess &header)
{
	uint8_t tempBuf[256];
	memset((void*)tempBuf, 0xff, 256);
	//найдем свободный сектор
	//рассчитаем кол-во необходимых секторов
	uint32_t countSectors = calcCountSectors(header);
	//посчитаем кол-во свободных секторов
	if( countSectors > countFreeSectors() )
		return false;

	for(int i = 0; i < MAX_SECTORS; i++)
	{
		if( headerList[i] == 0xffff )
		{
			numProc = i;
			break;
		}
	}
//	проверим, что процесс влезет в свободнуу память флэшь
//находим цепочку секторов и записываем в начало каждого сектора предывцщий и следующий сектор
//	uint8_t sensors = countSensor(header);

	if( countSectors == 1 )
	{
		for(int i = 0; i < MAX_SECTORS; i++)
		{
			if( (flashMap[i][0] == 0xffff) && (flashMap[i][1] == 0xffff) )
			{
				headerList[countProc++] = i;
				tempBuf[0] = 0xfe; //признак того, что страница первая
				tempBuf[1] = 0xff;
				tempBuf[2] = 0xfd; //признак того, что страница последняя
				tempBuf[3] = 0xff;
				memcpy((void*)&tempBuf[4], (void*)&header,
						sizeof(HeaderProcess));
				tempBuf[4 + sizeof(HeaderProcess)] = i;
				tempBuf[4 + sizeof(HeaderProcess) + 1] = i >> 8;
				flashMx25Write((uint8_t*)tempBuf, i * 4096);

				//flashMx25Read((void*)tempBuf, i * 4096, 256);

				flashMap[i][0] = 0xfffe;
				flashMap[i][1] = 0xfffd;

				return true;
			}
		}
	}
	else
	{
		if( (4 + sizeof(HeaderProcess) + countSectors * 2) > 4096 )
			return false; //если страниц много и они не влезут в один sector, то вертаемся
		uint16_t j = countSectors;
		uint16_t *coilSectors = new uint16_t[countSectors];
		uint16_t numSec = 0;
		for(int i = 0; i < MAX_SECTORS; i++)
		{
			if( (flashMap[i][0] == 0xffff) && (flashMap[i][1] == 0xffff) )
			{
				coilSectors[numSec++] = i;
				if( --j == 0 )
					break;
			}
		}
		if( j == 0 )
		{
			for(int n = 0; n < countSectors; n++)
			{
				if( n == 0 )
				{
					headerList[countProc++] = coilSectors[n];
					tempBuf[0] = 0xfe; //признак того, что страница первая
					tempBuf[1] = 0xff;
					flashMap[coilSectors[n]][0] = 0xfffe;
					memcpy((void*)&tempBuf[4], (void*)&header,
							sizeof(HeaderProcess));

					uint16_t allSize = 4 + sizeof(HeaderProcess);
					void *p = (void*)&tempBuf[allSize];
					uint32_t addInFlash = coilSectors[n] * 4096;
					allSize = countSectors * sizeof(uint16_t);
					do
					{ //вычислим оставшияся размер блока
						uint16_t tempSize = 256 - ((uint8_t*)p - tempBuf);
						if( allSize < tempSize )
							tempSize = allSize;
						allSize -= tempSize;
						memcpy(p, (void*)coilSectors, tempSize);
						flashMx25Write((uint8_t*)tempBuf, addInFlash);
						addInFlash += 256;
						memset((void*)tempBuf, 0xff, 256);
						p = (void*)tempBuf;
					} while(allSize > 0);
				}
				else
				{
					tempBuf[0] = coilSectors[n - 1];
					tempBuf[1] = coilSectors[n - 1] >> 8;
					flashMap[coilSectors[n]][0] = coilSectors[n - 1];

				}
				if( n == (countSectors - 1) )
				{
					tempBuf[2] = 0xfd; //признак того, что страница последняя
					tempBuf[3] = 0xff;
					flashMap[coilSectors[n]][1] = 0xfffd;
				}
				else
				{
					tempBuf[2] = coilSectors[n + 1];
					tempBuf[3] = coilSectors[n + 1] >> 8;
					flashMap[coilSectors[n]][1] = coilSectors[n + 1];
				}
				flashMx25Write((uint8_t*)tempBuf, coilSectors[n] * 4096);
			}
			delete[] coilSectors;
			return true;
		}
		delete[] coilSectors;
	}
	return false;
}

//записывает одну точку процесса во флэшку
void saveResult(float *result, int countSensers)
{
	uint8_t *resultVoid = (uint8_t*)result;
	uint32_t address = getAdrCurPoint();
	uint8_t tempBuf[256];
	memset((void*)tempBuf, 0xff, 256);
	if( (address % 4096) == 4 )
	{ //записать адрес предыдущего сектора и адрес следующего сектора в начало сектора
		uint16_t cursector = address / 4096;
		tempBuf[0] = flashMap[cursector][0];
		tempBuf[1] = flashMap[cursector][0] >> 8;
		tempBuf[2] = flashMap[cursector][1];
		tempBuf[3] = flashMap[cursector][1] >> 8;
	}
	uint32_t remainder = address % 256;
	uint16_t pointSize = countSensers * sizeof(float);

	if( (remainder + pointSize) > 256 )
	{ //первую половину
		uint16_t firstSize = 256 - remainder;
		memcpy((void*)&tempBuf[remainder], (void*)resultVoid, firstSize);
		flashMx25Write(tempBuf, 256 * (address / 256));
//пишем вторую половину
		memset((void*)tempBuf, 0xff, 256);
		address += firstSize;
		remainder = address % 256;
		if( (address % 4096) == 0 )
		{ //записать адрес предыдущего сектора и адрес следующего сектора в начало сектора
			uint16_t cursector = address / 4096;
			tempBuf[0] = flashMap[cursector][0];
			tempBuf[1] = flashMap[cursector][0] >> 8;
			tempBuf[2] = flashMap[cursector][1];
			tempBuf[3] = flashMap[cursector][1] >> 8;
//			address += 4;
			remainder += 4;
		}
		uint16_t secondSize = pointSize - firstSize;
		resultVoid += firstSize;
		memcpy((void*)&tempBuf[remainder], (void*)resultVoid, secondSize);
		flashMx25Write(tempBuf, address);
	}
	else
	{ //не выходим за размер блока в 256 байт
		memcpy((void*)(&tempBuf[remainder]), (void*)resultVoid, pointSize);
		flashMx25Write(tempBuf, 256 * (address / 256));
	}
	currProcessCount++;
	if( currProcessCount >= currProcessHeader.count )
	{ //кончим процесс
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		closeProc();
		stateProcess = 2;
//		xTimerStop(timerMesuring, 100);
	}
}

uint32_t getProcessPeriod()
{
	return currProcessHeader.period;
}

uint32_t getAdrCurPoint()
{
//расчитать размер заголовка
	uint32_t headerSize = sizeof(HeaderProcess);
//расчитать размер цепочки секторов
	uint32_t countSectors = calcCountSectors(currProcessHeader);
	uint32_t coinSectorSize = countSectors * 2; //размер цепочки адресов секторов
//расчитать размер данных уже записанных
	uint32_t dataSize = currProcessCount * countSensor(currProcessHeader)
			* sizeof(float);
//размер заголовка + сектора
//	uint16_t offset = 4 + headerSize + coinSectorSize;
//посчитать адрес куда нужно писать
	uint32_t sizeData = headerSize + coinSectorSize + dataSize;
	//почитаем размер данных в первом сеторе
	uint32_t numSector = sizeData / (4096 - 4); // = Целое и остаток
	numSector++; // целое+1 = это номер сектора в цепочке

//находим номер сектора numSector
	uint16_t sector = headerList[numProc];
	while(--numSector)
	{
		if( sector >= 4096 )
			asm("nop");
		sector = flashMap[sector][1];
	}
	uint32_t remainder = sizeData % (4096 - 4);
	uint32_t address = sector * 4096 + remainder + 4; //	свмещение в последнем секторе = остаток + 4
	return address;
}

uint8_t countSensor(const HeaderProcess& header)
{
	uint8_t countSens = 0;
	for(int i = 0; i < 8; i++)
	{
		if( header.config.sensorType[i] < GT_Absent )
			countSens++;
	}
	return countSens;
}

extern "C" void RTC_Alarm_IRQHandler()
{
	if( RTC_GetITStatus(RTC_IT_ALRA) != RESET )
	{
		RTC_ClearITPendingBit(RTC_IT_ALRA);

		//RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		EXTI_ClearITPendingBit(EXTI_Line17);
		/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		if( stateProcess == 1 )
			xEventGroupSetBitsFromISR(xEventGroup, FLAG_MESUR,
					&xHigherPriorityTaskWoken);
		//setNewAlarmRTC(5); 		//перезапустим таймер
		//setNewAlarmRTC(currProcessHeader.period);

	}
}
