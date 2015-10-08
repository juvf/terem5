/*
 ������ � �������� ��������� HIH-3610 (HIH-4000)
 Vout = Vsupply (0.0062(Sensor RH) + 0.16), typical @ 25 �C
 Sensor RH = (Vout - 0.8) / 0.031
 = (Vout - 0.16*Vsupply) / (0.0062*Vsupply) = //Vsupply=5
 = (Vout/Vsupply - 0.16) / 0.0062 =
 = (Vout-0.16*Vsupply) * (161.29/Vsupply)
 True RH = (Sensor RH)/(1.0546-0.00216T), T in �C
 C 13.11.2008 ���������� ������������ ���������� A0(U0), A1(KU)
 */

#include "main.h"
#include "GaugeHIH3610.h"

#define Vsupply 4.0   //���������� ������� �������
#define U0      a[0]   //�� 13.11.2008 ���� (0.16*Vsupply), ������� Rh=(2*curU-U0)*Ku
#define Ku      a[1]   //�� 13.11.2008 (161.29/Vsupply)

//�������������� ���������� � ������������� ��������� -------------------------
float HIH3610_Rh(float curU, float T, float *a)
{
//	float *a1 = a0 - 1;
	float Rh;

	//�������� ������� ������������� ����������
	if((U0 < 0.5) || (U0 > 1.5))
		U0 = 0.85;
	if((Ku < 0.02) || (Ku > 0.05))
		Ku = 0.031;
	//Rh ��� 25��
	//��� ��� AD7792 ���������� ����� �������� �� 4, ��� AD7708 - �� 2
//	if(gFlags.AD7792)
		Rh = (curU * (4 * 5 / Vsupply) - U0) / Ku; //4 - ��������, 5/Vsupply - ���� ������� ������� 4�
//	else
//		Rh = (curU * (2 * 5 / Vsupply) - U0) / Ku; //2 - ��������, 5/Vsupply - ���� ������� ������� 4�
	if(Rh < 0)
		return flErrorCode;
	//��������� �� �����������
	if(T != flErrorCode)
		Rh /= (1.0546 - 0.00216 * T);
	//�������� �� ������������
	if(Rh > 110.0)
		return flErrorCode;
	if(Rh > 100.1)
		return 100.1;
	return Rh;
}

