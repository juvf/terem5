/*
 ������ � ���������� �������� ����������� HEL-700
 */
#include "GaugeHEL700.h"
#include "main.h"
#include "Sensor.h"
#include "../configTerem.h"
#include <math.h>

//������������ �������� T�(R, ���) ��� ������� HEL-700
//������� ���������� ������������� + � - �����������, +-0,002 ��. ������
static const float HEL700_Plus_a4[] = {
		0.165024255, 0.120454293, 9.612479410, 242.2041992, -252.101939 }; //A4, A3, A2, A1, A0
static const float HEL700_Minus_a4[] = {
		1.909069965, -13.4670783, 38.67544987, 217.7644066, -244.883268 };

//�������������� ���������� � ����������� -------------------------------------
float HEL700_Termo(float curU, unsigned char Channel)
{
	float T;
	float R = curU;
//	if(gFlags.AD7792)
//		R = curU;
//	else
//		R = curU / (/*DataFlash1.Vref*/2.5 / 10.0); //������������� ������� � ��� (�/��)
	if(R < 0.2)
		return flErrorCode;                        //���� -200 �C
	else if(R < 1.0)
		T = Polinom4(R, HEL700_Minus_a4);            //T<0
	else if(R < 2.755)
		T = Polinom4(R, HEL700_Plus_a4);             //T>0
	else
		return flErrorCode;                        //���� +500 �C

	//��������� �������������� ���������
	return Polinom4(T, koeffsAB.koef[Channel].a);
}

