/*
 Термопара с индивидуальной калибровкой
 */

#include "main.h"
#include "../configTerem.h"
#include "Sensor.h"

//Преобразование напряжения термопары в температуру ---------------------------
float T48_Termo(float curU, float T, unsigned char Channel)
{
	float dU;
	if((curU > 0.1) || (curU < -0.1))
		return flErrorCode; //Не в диапазоне U, это ошибка измерения
	if((T < -50) || (T > 150))
		return flErrorCode; //Не в диапазоне T, это ошибка измерения
	//Чтение коэффициентов линеаризации из Flash
//	AB_Read(Channel, 1);
	//Вычисление напряжения компенсации по формуле полинома 4-го порядка
	dU = Polinom4(T, &koeffsAB.koef[Channel].b[0]);	//AB[5]);
	if((dU > 0.1) || (dU < -0.1))
		return flErrorCode; //Не в диапазоне U, это ошибка измерения
	curU += dU;
	//Пересчет по формуле полинома 4-го порядка
	return Polinom4(curU, koeffsAB.koef[Channel].a);
}

//Запись коэффициентов ХК-термопары по умолчанию ------------------------------
void DefaultKoefHK(void)
{
	/*
	 //Диапазон -50...+300°, погрешность 0,5°С
	 A4=B4=0;
	 A3=+2.916E+06;
	 A2=-1.811E+05;
	 A1=+1.579E+04;
	 A0=-2.972E-01;
	 B3=-7.446E-11;
	 B2=+5.916E-08;
	 B1=+6.320E-05;
	 B0=-7.382E-08;
	 */
	for(unsigned char Channel = 0; Channel < 8; Channel++)
	{
		//Диапазон -50...+150°, погрешность 0,1°С
		koeffsAB.koef[Channel].a[0] = 0;
		koeffsAB.koef[Channel].a[4] = 0;
		koeffsAB.koef[Channel].b[0] = 0;
		koeffsAB.koef[Channel].a[1] = +4.648E+06;  //A3
		koeffsAB.koef[Channel].a[2] = -2.012E+05;  //A2
		koeffsAB.koef[Channel].a[3] = +1.578E+04;  //A1
		koeffsAB.koef[Channel].b[1] = -4.630E-11;  //B3
		koeffsAB.koef[Channel].b[2] = +5.798E-08;  //B2
		koeffsAB.koef[Channel].b[3] = +6.320E-05;  //B1
		koeffsAB.koef[Channel].b[4] = -1.270E-07;  //B0
		//	AB_Write(Ad, Channel, 1);
	}
	saveKoeffAB();
}

