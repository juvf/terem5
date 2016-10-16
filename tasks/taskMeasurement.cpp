/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 авг. 2015 г.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "clock.h"
#include "stm32f4xx_rtc.h"
#include  "../../structCommon.h"
#include  "configTerem.h"
#include "sensor/Sensor.h"
#include "Process.h"
#include "../../adc.h"
#include "sensor/ds1820.h"
#include "main.h"
#include <math.h>

float valueSens[16];

void taskMeasurement(void *context)
{ //задача запускаемая по таймера для измерения точки процесса.
//нужно пройти по всем подключенным датчика процесса и опросить их.
//имеем массив датчиков.... у каждого датчика есть настройка - включен он или блять не включен и его тип
//проверяем енто
	vTaskDelay(1000);
	while(1)
	{

		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, FLAG_MESUR,
		pdFALSE, pdTRUE, 3000);
		if( (uxBits & FLAG_MESUR) == FLAG_MESUR )
		{
			ledRedOn();
			setNewAlarmRTC(getProcessPeriod()); 		//перезапустим таймер
			musuring();
			vTaskDelay(300);
			ledRedOff();
			xEventGroupClearBits(xEventGroup, FLAG_MESUR);
			xEventGroupSetBits(xEventGroup, FLAG_SLEEP_MESUR);
		}
	}

}

void timerMeasurement(xTimerHandle xTimer)
{
// Код функции таймера
	//musuring();
	//xEventGroupSetBits(xEventGroup, FLAG_MESUR);
	//return;
}

void musuring()
{
	static ResultMes result1;
	int j = 0;
	//захватим симафор АЦП
	xSemaphoreTake(semaphAdc, portMAX_DELAY);
	//выставим флаг готовности новых данных
	tempOfDs1820 = readtemp() + configTerem.deltaT;

	ep1_On();
	epa_On();
	for(int i = 0; i < 8; i++)
	{	//опрос всех каналов
		result1 = readSenser(i);
		if( isnan(result1.u) == 0 )
		{
			if( configTerem.sensorType[i] == GT_SHT21 )
				valueSens[j++] = result1.uClear;
			else
				valueSens[j++] = result1.p;
		}
	}
	epa_Off();
	ep1_Off();

	xEventGroupSetBits(xEventGroup, FLAG_IS_READY_MES);
	//освободим симафор АЦП
	xSemaphoreGive(semaphAdc);

	//записываем результат
	saveResult(valueSens, j);
}

