/*
 * Process.cpp
 *
 *  Created on: 05 ���. 2015 �.
 *      Author: juvf
 */
#include "Process.h"
#include "stm32f4xx_rtc.h"
#include "../osConfig.h"
#include "taskMeasurement.h"
#include "Checksum.h"
#include "../flashMx25.h"
#include  <string.h>

uint16_t numProc = 0xffff; //����� �������� �������� � headerList[]
HeaderProcess currProcessHeader; //��������� �������� ��������
uint32_t currProcessCount; //���-�� ���������� �����
uint8_t stateProcess; //������� ��������� �������� 0-��� ��������, 1-��� �������, 2-����������, 3-���� ������

uint16_t headerList[MAX_SECTORS] = { 0xffff }; //
uint16_t countProc = 0; //���-�� ��������� � ������, ������� �������

struct Header
{
	uint16_t preNext[2];
	HeaderProcess header;
};

//uint32_t getAdrCurPoint()
//{
//
//}

//������������ ������ � ���������� ������� ���������� ���������� �������� headerList[]
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
	uint32_t countSectords = calcCountSectors(header.header);

	//������ ������� ��������
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

uint32_t calcCountSectors(const HeaderProcess &header)
{
	uint16_t countSenser = 0;
	for(int i = 0; i < 8; i++)
	{
		if(header.config.sensorType[i] < GT_Absent)
			countSenser++;
	}
	uint32_t sizeOfProcessData = 4 * header.count * countSenser;
//���������� ���-�� ��������
	uint32_t remainder = sizeOfProcessData % 4096;
	uint32_t countSectords = 1 + (sizeOfProcessData / 4096);
	uint16_t sizeOfHeader = sizeof(Header) + 4 + (countSectords * 2); //������ ��������� � ������� ������� ��������
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

	uint32_t countSectords = calcCountSectors(header.header);
	//����� ������� ��������
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
	//������� ���� ������ headerList
	for(int i = number + 1; i < MAX_SECTORS; i++)
	{
		headerList[number - 1] = headerList[number];
		if(headerList[number] == 0xffff)
			break;
	}
	headerList[MAX_SECTORS - 1] = 0xffff;
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
			currProcessHeader.count = buffer[10] | (buffer[11] << 8)
					| (buffer[12] << 16) | (buffer[13] << 24);
			if(buffer[0] == 0xff)
			{ //����������� ������ �������

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


				//��������� ��������� ��������
				if(allocMemForNewProc(currProcessHeader))
				{
					currProcessCount = 1;
					musuring();
					if( xTimerChangePeriod(timerMesuring, per * 1000,
							100) == pdFAIL)
						buffer[0] = 1;
					else
					{
						xTimerReset(timerMesuring, 100);
						buffer[0] = 0;
						stateProcess = 1;
					}
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
	{ //������ ��� ���, ��� ��� ������, ������� ������ ������ ��������� ��������
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
	uint8_t tempBuf[256];
	memset((void*)tempBuf, 0xff, 256);
	//������ ��������� ������
//���������� ���-�� ����������� ��������
	uint32_t countSectors = calcCountSectors(header);
	//��������� ���-�� ��������� ��������
	if(countSectors > countFreeSectors())
		return false;

	for(int i = 0; i < MAX_SECTORS; i++)
	{
		if(headerList[i] == 0xffff)
		{
			numProc = i;
			break;
		}
	}
//	��������, ��� ������� ������ � ��������� ������ �����
//������� ������� �������� � ���������� � ������ ������� ������� ���������� � ��������� ������
	uint8_t sensors = countSensor(header);

	uint16_t j = sensors;
	uint16_t prePrePage = 0xfffe;
	uint16_t prePage = 0xfffe;
	for(int i = 0; i < MAX_SECTORS; i++)
	{
		if((flashMap[i][0] == 0xffff) && (flashMap[i][1] == 0xffff))
		{
			if(countSectors == 1)
			{
				headerList[countProc++] = i;
				tempBuf[0] = 0xfe; //������� ����, ��� �������� ������
				tempBuf[1] = 0xff;
				tempBuf[2] = 0xfe; //������� ����, ��� �������� ���������
				tempBuf[3] = 0xff;
				flashMx25Write((uint8_t*)tempBuf, i);
				break;
			}
			else
			{
				if(j == sensors)
					headerList[countProc++] = i;
				tempBuf[0] = prePrePage;
				tempBuf[1] = prePrePage >> 8;
				tempBuf[2] = i;
				tempBuf[3] = i >> 8;
				//������ � ������������� preNext
				flashMx25Write((uint8_t*)tempBuf, prePage);
				prePrePage = prePage;
				prePage = i;
				j--;
				if(j == 0)
				{
					//����� ��������� ��������
					tempBuf[0] = prePage; //������� ����, ��� �������� ������
					tempBuf[1] = prePage >> 8;
					tempBuf[2] = 0xfe; //������� ����, ��� �������� ���������
					tempBuf[3] = 0xff;
					flashMx25Write((uint8_t*)tempBuf, i);
					break;
				}
			}
		}
	}
	return true;
}

//���������� ���� ����� �������� �� ������
void saveResult(float *result, int countSensers)
{
	uint8_t *resultVoid = (uint8_t*)result;
	uint32_t address = getAdrCurPoint();
	uint8_t tempBuf[256];
	memset((void*)tempBuf, 0xff, 256);
	if((address % 4096) == 4)
	{ //�������� ����� ����������� ������� � ����� ���������� ������� � ������ �������
		uint16_t cursector = address / 4096;
		tempBuf[0] = flashMap[cursector][0];
		tempBuf[1] = flashMap[cursector][0] >> 8;
		tempBuf[2] = flashMap[cursector][1];
		tempBuf[3] = flashMap[cursector][1] >> 8;
	}
	uint32_t remainder = address % 256;
	uint16_t pointSize = countSensers * sizeof(float);
	if((remainder + pointSize) > 256)
	{ //������ ��������
		uint16_t firstSize = 256 - remainder;
		memcpy((void*)&tempBuf[remainder], (void*)resultVoid, firstSize);
		flashMx25Write(tempBuf, address);
		//����� ������ ��������
		memset((void*)tempBuf, 0xff, 256);
		address += firstSize;
		if((address % 4096) == 0)
		{ //�������� ����� ����������� ������� � ����� ���������� ������� � ������ �������
			uint16_t cursector = address / 4096;
			tempBuf[0] = flashMap[cursector][0];
			tempBuf[1] = flashMap[cursector][0] >> 8;
			tempBuf[2] = flashMap[cursector][1];
			tempBuf[3] = flashMap[cursector][1] >> 8;
			address += 4;
		}
		remainder = address % 256;
		uint16_t secondSize = pointSize - firstSize;
		resultVoid += firstSize;
		memcpy((void*)&tempBuf[remainder], (void*)resultVoid, secondSize);
		flashMx25Write(tempBuf, address);
	}
	else
	{ //�� ������� �� ������ ����� � 256 ����
		memcpy((void*)(&tempBuf[remainder]), (void*)resultVoid, pointSize);
		flashMx25Write(tempBuf, address);
	}
	currProcessCount++;
	if(currProcessCount == currProcessHeader.count)
	{//������ �������
		stateProcess = 2;
		xTimerStop(timerMesuring, 100);
	}
}

uint32_t getAdrCurPoint()
{
	//��������� ������ ���������
	uint32_t headerSize = sizeof(HeaderProcess);
	//��������� ������ ������� ��������
	uint32_t countSectors = calcCountSectors(currProcessHeader);
	uint32_t coinSectorSize = countSectors * 2; //������ ������� ������� ��������
	//��������� ������ ������ ��� ����������
	uint32_t dataSize = currProcessCount * countSensor(currProcessHeader)
			* sizeof(float);
	//��������� ����� ���� ����� ������
	uint32_t sizeData = headerSize + coinSectorSize + dataSize;
	uint32_t numSector = sizeData / (4096 - 4); // = ����� � �������
	numSector++; // �����+1 = ��� ����� ������� � �������

	//������� ����� ������� numSector
	uint16_t sector = headerList[numProc];
	while(--numSector)
	{
		sector = flashMap[sector][1];
	}
	uint32_t remainder = sizeData % (4096 - 4);
	uint32_t address = sector * 4096 + remainder + 4; //	��������� � ��������� ������� = ������� + 4
	return address;
}

uint8_t countSensor(const HeaderProcess& header)
{
	uint8_t countSensor = 0;
	for(int i = 0; i < 8; i++)
	{
		if(header.config.sensorType[i] < GT_Absent)
			countSensor++;
	}
	return countSensor;
}
