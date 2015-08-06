/*
 * taskMeasurement.cpp
 *
 *  Created on: 06 авг. 2015 г.
 *      Author: anisimov.e
 */

#include "taskMeasurement.h"
#include "stm32f4xx_conf.h"

//void taskMeasurement(void *context)
//{ //задача запускаемая по таймера для измерения точки процесса.
////нужно пройти по всем подключенным датчика процесса и опросить их.
////имеем массив датчиков.... у каждого датчика есть настройка - включен он или блять не включен и его тип
////проверяем енто
//
//}

void taskMeasurement(xTimerHandle xTimer)
{
// Код функции таймера
	musuring();
	return;
}

void musuring()
{
	static int a = 0;

	if( a == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
		GPIO_SetBits(GPIOA, GPIO_Pin_9);
		a = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
		GPIO_ResetBits(GPIOA, GPIO_Pin_9);
		a = 0;
	}
}

