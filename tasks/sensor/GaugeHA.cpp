/*
 �������������� ���������� � ����������� ��� ��-���������

 1. ���������� �������� ����������� ���������
 ���� � ��� ���������
 2. ����������� ��������� ���� ���������������
 � ��� ��������� ���� ���������
 3. ����������� ��� �������������� ����
 ���� = ��� + �����
 4. �� ��� �������������� ���� �����������
 ��� �����������
 */
#include "GaugeHA.h"
#include "Sensor.h"
#include "main.h"

//������� ����������� ���� �� ��������� � ��� ---------------------------------
//����������� - ������� �������, ��� - �
//                       B3               B2              B1           B0

//-1,008159E-10x3 + 3,075175E-08x2 + 3,965641E-05x + 3,538462E-06


//const float _B__200[] = { -1.008159E-10, 3.075175E-8, 3.965641E-5, 3.538462E-6 }; //�� -200 �� -100
const float _B__40[] = {
		-1.2080784e-10, 2.7021065e-8, 0.0000394254, -0.0000007932 };
const float _B__10[] = { 6.4500000e-8, 3.9200e-8, 0.0001104500, 0.0 };
const float _B_0[] =
		{ -5.5555556e-11, 2.1785714e-8, 0.0000394925, -0.0000000238 };
const float _B_60[] = {
		-9.5117845e-11, 2.1803752e-8, 0.0000398584, -0.0000137121 };
float HA_T2E(float T)
{
	const float *Ptr;
//	if(T < -100.0)
//		Ptr = _B__200; //-200...-100
//	else
		if(T < -10.0)
		Ptr = _B__40; //-100...-10, ������ 0.02 ��.�
	else if(T < 0.0)
		Ptr = _B__10; //-10...0, ������ ? ��.�
	else if(T < 60.0)
		Ptr = _B_0;   //0...+60, < 0.1 ��.�
	else
		Ptr = _B_60;  //+60...+140, < 0.1 ��.�
	return Polinom3(T, Ptr);
}

//������� ��� �� ��������� � ����������� ���� ---------------------------------
//��� - �, ����������� - ������� �������
//                       B3              B2             B1         B0
const float B__200[] = { 1936205168.74072, 21695785.63138, 115468.834219, 123.377116 };
const float B__100[] = { 0.2265484e9, 0.3723747e6, 26.9060204e3, 1.0330 };
const float B__50[] = { 0.0899775e9, -0.4328153e6, 25.3297836e3, 0.0048 };
const float B_30[] = { 0.0349640e9, -0.3407548e6, 25.1976760e3, 0.1354 };
const float B_150[] = { -0.0146507e9, 0.3173305e6, 22.7264582e3, 1.9397 };
const float B_300[] = { 0.0023193e9, -0.1530755e6, 26.8254781e3, -8.8764 };
const float B_650[] = { 0.0013889e9, -0.0656319e6, 24.1381858e3, 18.259 };
const float B_900[] = { 0.0017135e9, -0.1221224e6, 27.0060729e3, -26.973 };
float HA_E2T(float E)
{
	if(E < -3.554e-3)
		return Polinom3(E, B__200); //-200...-100
	if(E < -1.9e-3)
		return Polinom3(E, B__100); //-100...-50, ������ 0.03 ��.C
	if(E < 1.2e-3)
		return Polinom3(E, B__50);  //-50...+30, ������ 0.02 ��.C
	if(E < 6.1e-3)
		return Polinom3(E, B_30);   //+30...+150, ������ 0.021 ��.C
	if(E < 12.2e-3)
		return Polinom3(E, B_150);  //+150...+300, ������ 0.023 ��.C
	if(E < 27.0e-3)
		return Polinom3(E, B_300);  //+300...+650, ������ 0.023 ��.C
	if(E < 37.3e-3)
		return Polinom3(E, B_650);  //+650...+900, ������ 0.008 ��.C
	return Polinom3(E, B_900);  //+900...+1200, ������ 0.014 ��.C
}

//�������������� ���������� � ����������� -------------------------------------
//����: U - ��� ��������� � �, Tcj - ����������� ��������� ���� � ��.�
float HA_Termo(float U, float Tcj)
{
	float dU;
	if(Tcj == flErrorCode)
		return flErrorCode;        //������ ��������� ����������� ��������� ����
	dU = HA_T2E(Tcj);                          //����������� ��������� ���� � ��
	U += dU;
	if((U < -0.0061) || (U > 0.053))
		return flErrorCode;   //����������� �� � ���������, ��� ������ ���������
	else
		return HA_E2T(U);           //����������� � ������ ��� �� ��������� ����
}

