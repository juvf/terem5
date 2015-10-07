/*
 Gauge HeatFlow
 */

#include "GaugeHeatFlow.h"
#include "Sensor.h"
#include "../configTerem.h"
#include "main.h"

//�������������� ���������� � �������� ����� ----------------------------------
float HF_Flow(float curU, float *a0)
{
	float *a1 = a0 - 1;
	float Flow = *a1 * curU * 1000.0;
	if((Flow > 3000.0) || (Flow < -3000.0))
		return flErrorCode; //�� � ���������, ��� ������ ���������
	return Flow;
}

//�������������� ���������� � �������� �����, �� �������� ������ --------------
float HF_Flow48(float curU, unsigned char Channel)
{
	if((curU > 0.3) || (curU < -0.3))
		return flErrorCode;       //�� � ���������, ��� ������ ���������
	//������ ������������� ������������ �� Flash
//	AB_Read(Ad, Channel, 0);
	//�������� �� ������� �������� 4-�� �������
	return Polinom4(curU, koeffsAB.koef[Channel].a);
}

