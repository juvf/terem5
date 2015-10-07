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

#include "main.h"

//������� ����������� ���� �� ��������� � ��� ---------------------------------
//����������� - ������� �������, ��� - �
//                          A3             A2             A1              A0
const float _A__100[] = {
		-1.0889528e-10, 5.6611846e-8, 0.0000631224, -0.0000033986 };
const float _A__50[] = { -9.2592593e-11, 5.8055556e-8, 0.0000631946, 0.0 };
const float _A__00[] =
		{ -4.3697848e-11, 5.4828128e-8, 0.0000633782, -0.00000132 };
float HK_T2E(float T)
{
	if(T < -50.0)
		return Polinom3(T, _A__100);  //-100...-50, ������ < 0.02 ��.�
	if(T < 0.0)
		return Polinom3(T, _A__50);   //-50...0, ������ < 0.1 ��.�
	else
		return Polinom3(T, _A__00);   //0...+200, < 0.1 ��.�
}

//������� ��� �� ��������� � ����������� ���� ---------------------------------
//��� - �, ����������� - ������� �������.
//                          A3             A2             A1           A0
const float A__100[] = { 0.0508945e9, 0.0997230e6, 16.8871416e3, 1.2076 };
const float A__50[] = { 0.0083612697e9, -0.23679662e6, 15.849099e3, -0.0009 };
const float A_110[] = { 0.0036992e9, -0.1875185e6, 15.749551e3, -0.1633 };
const float A_160[] = { 0.0014716e9, -0.1242889e6, 15.148516e3, 1.7555 };
const float A_300[] = { 0.00081904066e9, -0.0880636e6, 14.482406e3, 5.8024 };
const float A_550[] = { -0.0001122965e9, 0.0189071e6, 10.355013e3, 59.348 };
const float A_750[] = { 0.0019993e9, -0.3403786e6, 30.715420e3, -324.91 };
float HK_E2T(float E)
{
	if(E < -4.1e-3)
		return Polinom3(E, A__100);   //-100...-50, 0.02 ��.C
	if(E < -3.0e-3)
		return 0.23090695e9 * E * E * E + 203.17659e-6 / E / E - 66.369407; //-70...-50, ������ �� 0,09 ? ��.C
	if(E < 6.8e-3)
		return Polinom3(E, A__50); //-50...+100, 0.04 ��.C
	if(E < 10.6e-3)
		return Polinom3(E, A_110); //+110...+150, 0.03 ��.C
	if(E < 18.6e-3)
		return Polinom3(E, A_160); //+160...+250, 0.03 ��.C
	if(E < 42.0e-3)
		return Polinom3(E, A_300); //+300...+500, 0.03 ��.C
	if(E < 60.0e-3)
		return Polinom3(E, A_550); //+550...+700, 0.03 ��.C
	return Polinom3(E, A_750); //+750...+800, 0.04 ��.C
}

//�������������� ���������� � ����������� -------------------------------------
//����: U - ��� ��������� � �, Tcj - ����������� ��������� ���� � ��.�
float HK_Termo(float U, float Tcj)
{
	float dU;
	if(Tcj == flErrorCode)
		return flErrorCode;        //������ ��������� ����������� ��������� ����
	dU = HK_T2E(Tcj);                          //����������� ��������� ���� � ��
	U += dU;
	if((U < -0.007) || (U > 0.070))
		return flErrorCode;   //����������� �� � ���������, ��� ������ ���������
	else
		return HK_E2T(U);           //����������� � ������ ��� �� ��������� ����
}

