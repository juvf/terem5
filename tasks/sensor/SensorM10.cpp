/*
 * SensorM10.cpp
 *
 *  Created on: 06 ���. 2015 �.
 *      Author: anisimov.e
 */

#include "SensorM10.h"
#include "../adc.h"

/*
 ������ � �������� ����������� (���������) MM-10

 ������������ ������������� ������������ a1_x, a0_x
 a1_x - �������������� �����������
 a0_x - ����������, ���������� ��� ���������� ������� "��������� ����"
 */

/*
 a1       a0
 ������ � �������� � 0001: L=5.034*U-7.395;  U=0.1986*L+1.469
 ������ � �������� � 0002: L=3.865*U-3.044;  U=0.2588*L+0.788
 ������ � �������� � 0003: L=3.835*U-4.131;  U=0.2607*L+1.077
 ������ � �������� � 0004: L=3.221*U-3.247;  U=0.3105*L+1.008
 */

//�������������� ���������� � ����������� -------------------------------------
float MM10_Length(float curU, float a0)
{
	curU *= 2.0 / 1.17;
	//��������� � ��
	return (curU - a0) * 5.5;
}

//�������������� ���������� � ����������� -------------------------------------
float MM20_Length(float curU, float *a0)
{
	curU *= 2.0 / 1.17;
	//��������� � ��
	return (curU - *a0) * 10.0;
}

//�������������� ���������� � ����������� -------------------------------------
float MM50_Length(float curU, float *a0)
{
	curU *= 2.0 / 1.17;
	//��������� � ��
	return (curU - *a0) * 25.0;
}

//�������������� ���������� � �����������, ������������� ������ ---------------
float Relocate_Length(float curU, float *a0)
{
	float *a1 = a0 - 1;
	curU *= 2.0 / 1.17;
	//��������� � ��
	return (curU - *a0) * *a1 / 2.0;
}

//���������� -> �����������, ����������� ������ -------------------------------
float RelInd_Length(float curU, float *a0)
{
	float *a1 = a0 - 1;
	curU *= 2.0 / 1.17;
	//��������� � ��          2� �����
	return curU * *a1 + *a0;
}
