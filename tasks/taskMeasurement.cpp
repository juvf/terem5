/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 авг. 2015 г.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "stm32f4xx_conf.h"
#include  "../../structCommon.h"
#include  "configTerem.h"
#include "sensor/Sensor.h"
#include "Process.h"
#include "../../adc.h"
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
				pdTRUE, pdTRUE, 1000);
		if( (uxBits & FLAG_MESUR) == FLAG_MESUR )
			musuring();
		else
		{
			xEventGroupSetBits(xEventGroup, FLAG_SLEEP_MESUR);
		}
	}

}

void timerMeasurement(xTimerHandle xTimer)
{
// Код функции таймера
	//musuring();
	xEventGroupSetBits(xEventGroup, FLAG_MESUR);
	return;
}

void musuring()
{
	static int a = 0;
	int j = 0;
	//захватим симафор АЦП
	xSemaphoreTake(semaphAdc, portMAX_DELAY);
	ep1_On();
	epa_On();
	for(int i = 0; i < 8; i++)
	{	//опрос всех каналы
		ResultMes result = readSenser(i);
		if( isnan(result.u) == 0 )
			valueSens[j++] = result.p;
//		switch( configTerem.sensorType[i])
//		{
//			//Датчики перемещения
//			case GT_MM10:
//				val = readAnalogSensor(i);
//				val = MM10_Length(val, configTerem.a[i][0]);
//				valueSens[j++] = val;
//				break;
//			case GT_MM20:
//				val = readAnalogSensor(i);
//				val = MM20_Length(val, configTerem.a[i][0]);
//				valueSens[j++] = val;
//				break;
//			case GT_MM50:
//				val = readAnalogSensor(i);
//				val = MM50_Length(val, configTerem.a[i][0]);
//				valueSens[j++] = val;
//				break;
//			case GT_HEL700:
//				val = getU_Ad7792(i);
//				val = HEL700_Termo(val, i);
//				valueSens[j++] = val;
//				break;
//			case GT_TermoHK:
//				val = getU_Ad7792(i);
//
//				valueSens[j++] = val;
//				break;
//			default:
//				break;
//		}

	}
	epa_Off();
	ep1_Off();
	//освободим симафор АЦП
	xSemaphoreGive(semaphAdc);

	//записываем результат
	saveResult(valueSens, j);

	if( a == 0 )
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
//		GPIO_SetBits(GPIOA, GPIO_Pin_9);
		a = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
		a = 0;
	}
}

