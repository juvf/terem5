/*
 Работа с тензометрическими датчиками массы/силы/давления
 Используются калибровочные коэффициенты a1_x, a0_x (масса/сила/давление)

 Работа с тензометрическими датчиками относительной деформации
 Используются коэффициенты A4-A0, B4-B0

 Для проволочных тензодатчиков предельное dL/L=1.5% (15000 мкм/м)

 При Ks=2
 1 бит АЦП соответствует 0.55 мкВ (или 1.1 мкм/м)
 При R=200 Ом шунт 100 кОм соответствует 0.4 Ом или dR/R=0.2% или dL/L=0.1%
 или смещение dU=500.5 мкВ или 1000 мкм/м или 820 ед. АЦП

 Сопротивление шунта для калибровки r=Rg/(Ks*e), где Rg-сопр. датчика,
 Ks-коэф. тензочувствительности, e=dL/L
 */

#include "GaugeTenso.h"
#include "../../structCommon.h"
#include "main.h"
#include <math.h>

extern float AB[10]; //Коэффициенты A4-A0, B4-B0
#define Ks     AB[0]   //Коэффициент тензочувствительности
#define R      AB[1]   //Сопротивление тензодатчика
#define Kcal0  AB[2]   //Коэффициент калибровки нуля
#define KcalF  AB[3]   //Коэффициент калибровки полной шкалы
#define Mode *(BYTE*)&AB[4] //Байт режима измерений
//1-результат - напряжение моста
//2-калибровка нуля и полной шкалы
//3-нормальная работа

//Значения резисторов шунтов (в Омах)
#define R1  100000
#define R2   50000
//Порог для изменения напряжения моста, Ом
#define RtrU   300
//Порог для изменения номинала шунта, Ом
#define RtrR   190

//static void TenzROn(void);

//Преобразование напряжения в массу/силу/давление -----------------------------
//Вычитается скалиброванное смещение в мВ, умножается на калибровочный коэффициент
float Tenso_1(float curU, float *a0)
{
	float *a1 = a0 - 1;
	return (curU - *a0) * *a1;
}

//Преобразование напряжения в относительную деформацию ------------------------
//Вход: Измеренное напряжение, физ. номер адаптера, номер канала компенсатора
//#pragma optimize=none
//float Tenso_2(float curU, BYTE Ad, BYTE Channel)
//{
//	float dRR, dLL;
//
//	//Чтение коэффициентов линеаризации из Flash
//	AB_Read(Ad, Channel, 0);
//	//Проверки на допустимость
//	if( Ks==0) return flErrorCode;
//	if( R==0) return flErrorCode;
//	//Режим установки нуля (вывод измеренного напряжения)
//	if( Mode == 1 )
//		return curU * 1000.0;
//	//Установка нуля, вычисление калибровочного значения
//	else if( Mode == 2 )
//	{
//		BYTE CurRange = 0;
//		//Сохранение значения калибровки нуля
//		Kcal0=curU;
//
//		//Установка номера адаптера и канала
//		SetAdChannel(Ad);
//		if( DataFlash1.Flags & 1 )
//			SetChannel(Channel);
//		//Включение шунта, установка нужного напряжения моста
//		TenzROn();
//		SetBrigeU();
//		IdleCPU(10);
//		//Измерение выходного напряжения сбалансированного моста с шунтом
//		if( gFlags.AD7708 )
//		{
//			ClrADC_CS();
//			curU = GetU_AD7708(&CurRange);
//			SetADC_CS();
//		}
//		else if( gFlags.AD7792 )
//			curU = GetU_AD7792(&CurRange, GT_Tenso_uE);
//		else
//		{
//			ClrADC_CS();
//			curU = GetU_AD7705(&CurRange);
//			SetADC_CS();
//		}
//		//Выключение шунта
//		TenzROff();
//		//Получение dR/R (dR вызвано подключением шунта R1 или R2, R известно от пользователя)
//		if( R>RtrR) dRR=-R/(R+R1);
//		else dRR=-R/(R+R2);
//		//Получение KcalF
//		curU -= Kcal0;
//		KcalF=dRR/curU;         // + = -/-
//		Mode = 1;                 //Калибровка окончена
//		//Запись полученных коэффициентов во Flash
//		AB_Write(Ad, Channel, 1);
//		return Kcal0*1000.0;
//
//		//Вычисление относительной деформации
//	}
//	else
//	{
//		curU-=Kcal0;  //Учет калибровки нуля
//		//Получение dR/R
//		dRR=curU*KcalF;
//		//Получение dL/L
//		dLL=dRR*1e6/Ks;
//		return dLL;
//	}
//}

//Включение нужного резистора шунта -------------------------------------------
//static void TenzROn(void)
//{
//	if( R>RtrR) TenzR1On(); //Больше порога -> больший резистор шунта
//	else TenzR2On();//Меньшее сопротивление -> наоборот
//}

////Выбор напряжения тензомоста -------------------------------------------------
//void SetBrigeU(void)
//{
//	if( R>RtrU) TenzUHigh(); //Больше порога -> большее напряжение
//	else TenzUTiny();//Меньшее сопротивление -> наоборот
//}

//Задание умалчиваемых коэффициентов ------------------------------------------
void DefaultKoefTenso(void)
{
}

#undef Ks
#undef R
#undef Kcal0
#undef KcalF
#undef Mode

/*
 Работа с тензометрическими датчиками с паспортными характеристиками
 */

//#define Full    AB[0]   //Значение полной шкалы в физических единицах
//#define Uref    AB[1]   //Значение полной шкалы в Вольтах
//#define Sens    AB[2]   //Чувствительность, мВ/В
//#define U0      AB[3]   //Смещение нуля, мВ
//#define Tcomp   AB[4]   //Коэфф. температурной компенсации
//#define Type    *(GaugeTensoType*)&AB[5] //Тип датчика, GaugeTensoType
//#define T0      AB[6]   //Температура установки нуля
//Преобразование напряжения в результат ---------------------------------------
//Вход: Измеренное напряжение, физ. номер адаптера, номер канала компенсатора
float Tenso_3(float curU, uint8_t Channel, float T)
{
	float dLL, Res;

	//Чтение коэффициентов линеаризации из Flash
//	AB_Read(Ad, Channel, 1);

	float Full = koeffsAB.koef[Channel].a[0]; //    AB[0]   //Значение полной шкалы в физических единицах
	float Uref = koeffsAB.koef[Channel].a[1]; //   AB[1]   //Значение полной шкалы в Вольтах
	float Sens = koeffsAB.koef[Channel].a[2]; //   AB[2]   //Чувствительность, мВ/В
	float U0 = koeffsAB.koef[Channel].a[3]; //     AB[3]   //Смещение нуля, мВ
	float Tcomp = koeffsAB.koef[Channel].a[4]; //  AB[4]   //Коэфф. температурной компенсации
	GaugeTensoType Type = *(GaugeTensoType*)&koeffsAB.koef[Channel].b[0]; //   *(GaugeTensoType*)&AB[5] //Тип датчика, GaugeTensoType
	float T0 = koeffsAB.koef[Channel].b[1]; //     AB[6]   //Температура установки нуля

	//Проверки на допустимость
	if( Full == 0 )
		return flErrorCode;
	if( !isnormal(Full) )
		return flErrorCode;
	if( Uref == 0 )
		return flErrorCode;
	if( !isnormal(Uref) )
		return flErrorCode;
	if( Sens == 0 )
		return flErrorCode;
	if( !isnormal(Sens) )
		return flErrorCode;

	//Если нет данных о температуре, не проводить термокомпенсацию
	if( !isnormal(T0) && (T0 != 0) )
		T0 = 25;
	if( T0 == flErrorCode )
		T0 = 25;
	if( !isnormal(T) && (T != 0) )
		T = T0;
	if( T == flErrorCode )
		T = T0;

	//Различная обработка по типам датчиков
	switch(Type)
	{

		//KM-100B, встраиваемый датчик напряжения (strain gauge), Tokio Sokki Kenkyujo Co., Ltd.
		case GTT_KM100B:
			curU -= U0;                 //Вычитание смещения нуля
			dLL = 2e6 * curU / Uref;    // uE в мкм/м (dR/R=4*dU/U, dL/L=dR/R/2)
			Res = dLL * Sens;                 // uE с коррекцией
			Res += (Tcomp - 9.3) * (T - T0);                 //Термокомпенсация

			//uE3 = Ce * uEi + (Cbeta - gamma) * dT
			//                 Tcomp  9.3*10-6
			return Res;

			//Весы
		case GTT_Weigher:
			curU -= U0;
			curU *= 1000.0;			//Перевод в мВ, вычитание смещения нуля
			Res = curU * Full / (Uref * Sens);
			return Res;

			//Неизвестный тип датчика
		default:
			return flErrorCode;
	}
}

////Установка нуля тензометрического датчика ------------------------------------
//void Tenso3SetZero(BYTE Ad, BYTE Chan)
//{
//	BYTE Comp;  //Номер канала компенсатора температуры
//	float Temp; //Температура
//
//	//Чтение коэффициентов из Flash
//	AB_Read(Ad, Chan, 1);
//	//Запись значения смещения нуля
//	U0=U[Ad][Chan];     //Смещение нуля, мВ
//
//	//Поиск температуры компенсатора в массиве результатов
//	Comp = *(Chan + &DataFlash1.DF_CompChan0);
//	Temp = Result[Ad][Comp];
//	if( !isnormal(Temp) && (Temp != 0) )
//		Temp = 0;
//	//Запись температуры компенсатора
//	T0=Temp;
//
//	//Запись коэффициентов обратно во Flash
//	AB_Write(Ad, Chan, 1);
//}

