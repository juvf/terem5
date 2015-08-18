/*
 * Sensor.cpp
 *
 *  Created on: 06 рту. 2015 у.
 *      Author: anisimov.e
 */

#include "Sensor.h"
#include "../../structCommon.h"

uint8_t typeOfSensor[9] = { GT_Absent };

float readSenser(uint8_t numChanel)
{
	float value;
	switch(typeOfSensor[numChanel])
	{
		case GT_MM10:
			value = 10.1;
			break;
		case GT_MM20:
			value = 20.2;
			break;
		case GT_MM50:
			value = 50.2;
			break;
		default:
			value = 0;
			break;
	}
	return value;
}

void switchOn(uint8_t numChanel)
{
	//E0 - PC12, E1 - PB7, A0 - PB5, A1 - PD7
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	switch(numChanel)
	{
		case 0:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 1:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 2:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 3:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOC, GPIO_Pin_12);
			break;
		case 4:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 5:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 6:
			GPIO_ResetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
		case 7:
			GPIO_SetBits(GPIOB, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			break;
	}
}

Sensor::Sensor(uint8_t num)
{
	numChanel = num;
	type = GT_Absent;
}

Sensor::~Sensor()
{
}

uint8_t Sensor::getType() const
{
	return type;
}

void Sensor::setType(uint8_t type_)
{
	type = type_;
}

bool Sensor::isEnabled() const
{

}

