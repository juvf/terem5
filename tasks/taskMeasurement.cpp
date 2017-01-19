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
#include "sensor/Dpg2.h"
#include <math.h>

//#define TEST_MES

float valueSens[16];

void taskMeasurement(void *context)
{ //задача запускаемая по таймера для измерения точки процесса.
//нужно пройти по всем подключенным датчика процесса и опросить их.
//имеем массив датчиков.... у каждого датчика есть настройка - включен он или блять не включен и его тип
//проверяем енто
	xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_MESURING);
	vTaskDelay(1000);
	while(1)
	{

		EventBits_t uxBits = xEventGroupWaitBits(xEventGroup, FLAG_MESUR,
		pdFALSE, pdTRUE, 3000);
		if( (uxBits & FLAG_MESUR) == FLAG_MESUR )
		{
			ledGreenOn();
			xEventGroupClearBits(xEventGroup, FLAG_NO_WORKS_MESURING);
			setNewAlarmRTC(getProcessPeriod()); 		//перезапустим таймер
			//проверим батарею, если меньше 3.2 В, то завершаем процесс.
			if( getBatValue() < 136 )
				commandStopProc();//float u = 3.0 * vBat * 2 / 255;
			else
				musuring();
			ledGreenOff();
			xEventGroupClearBits(xEventGroup, FLAG_MESUR);
			xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_MESURING);
		}
	}

}
extern uint32_t currProcessCount; //кол-во записанных точек
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
	for(int i = 0; i < 9; i++)
	{	//опрос всех каналов
		result1 = readSenser(i);
		if( isnan(result1.u) == 0 )
		{
#ifdef TEST_MES
			valueSens[j++] = currProcessCount + 1.0*j/10;
#else
			if( configTerem.sensorType[i] == GT_SHT21 )
				valueSens[j++] = result1.uClear;
			else
				valueSens[j++] = result1.p;
#endif
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

