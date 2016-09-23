/*
 Gauge HeatFlow
 */

#include "GaugeHeatFlow.h"
#include "Sensor.h"
#include "../configTerem.h"
#include "main.h"

//Преобразование напряжения в тепловой поток ----------------------------------
float HF_Flow(float curU, float k)
{
	float Flow = k * curU * 1000.0;
	if((Flow > 3000.0) || (Flow < -3000.0))
		return flErrorCode; //Не в диапазоне, это ошибка измерения
	return Flow;
}

//Преобразование напряжения в тепловой поток, по варианту ВНИИМС --------------
float HF_Flow48(float curU, unsigned char Channel)
{
	if((curU > 0.3) || (curU < -0.3))
		return flErrorCode;       //Не в диапазоне, это ошибка измерения
	//Чтение коэффициентов линеаризации из Flash
//	AB_Read(Ad, Channel, 0);
	//Пересчет по формуле полинома 4-го порядка
	return Polinom4(curU, koeffsAB.koef[Channel].a);
}

