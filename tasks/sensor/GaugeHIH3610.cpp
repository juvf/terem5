/*
 Работа с датчиком влажности HIH-3610 (HIH-4000)
 Vout = Vsupply (0.0062(Sensor RH) + 0.16), typical @ 25 °C
 Sensor RH = (Vout - 0.8) / 0.031
 = (Vout - 0.16*Vsupply) / (0.0062*Vsupply) = //Vsupply=5
 = (Vout/Vsupply - 0.16) / 0.0062 =
 = (Vout-0.16*Vsupply) * (161.29/Vsupply)
 True RH = (Sensor RH)/(1.0546-0.00216T), T in °C
 C 13.11.2008 использует коэффициенты калибровки A0(U0), A1(KU)
 */

#include "main.h"
#include "GaugeHIH3610.h"

#define Vsupply 4.0   //Напряжение питания датчика
#define U0      a[0]   //до 13.11.2008 было (0.16*Vsupply), формула Rh=(2*curU-U0)*Ku
#define Ku      a[1]   //до 13.11.2008 (161.29/Vsupply)

//Преобразование напряжения в относительную влажность -------------------------
float HIH3610_Rh(float curU, float T, float *a)
{
//	float *a1 = a0 - 1;
	float Rh;

	//Проверка наличия коэффициентов калибровки
	if((U0 < 0.5) || (U0 > 1.5))
		U0 = 0.85;
	if((Ku < 0.02) || (Ku > 0.05))
		Ku = 0.031;
	//Rh для 25°С
	//Для АЦП AD7792 напряжение нужно умножить на 4, для AD7708 - на 2
//	if(gFlags.AD7792)
		Rh = (curU * (4 * 5 / Vsupply) - U0) / Ku; //4 - делитель, 5/Vsupply - учет питания датчика 4В
//	else
//		Rh = (curU * (2 * 5 / Vsupply) - U0) / Ku; //2 - делитель, 5/Vsupply - учет питания датчика 4В
	if(Rh < 0)
		return flErrorCode;
	//Коррекция по температуре
	if(T != flErrorCode)
		Rh /= (1.0546 - 0.00216 * T);
	//Проверка на переполнение
	if(Rh > 110.0)
		return flErrorCode;
	if(Rh > 100.1)
		return 100.1;
	return Rh;
}

