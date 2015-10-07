/*
 ��������� � �������������� �����������
 */

#include "main.h"
#include "../configTerem.h"
#include "Sensor.h"

//�������������� ���������� ��������� � ����������� ---------------------------
float T48_Termo(float curU, float T, unsigned char Channel)
{
	float dU;
	if((curU > 0.1) || (curU < -0.1))
		return flErrorCode; //�� � ��������� U, ��� ������ ���������
	if((T < -50) || (T > 150))
		return flErrorCode; //�� � ��������� T, ��� ������ ���������
	//������ ������������� ������������ �� Flash
//	AB_Read(Channel, 1);
	//���������� ���������� ����������� �� ������� �������� 4-�� �������
	dU = Polinom4(T, &koeffsAB.koef[Channel].b[0]);	//AB[5]);
	if((dU > 0.1) || (dU < -0.1))
		return flErrorCode; //�� � ��������� U, ��� ������ ���������
	curU += dU;
	//�������� �� ������� �������� 4-�� �������
	return Polinom4(curU, koeffsAB.koef[Channel].a);
}

//������ ������������� ��-��������� �� ��������� ------------------------------
void DefaultKoefHK(void)
{
	/*
	 //�������� -50...+300�, ����������� 0,5��
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
		//�������� -50...+150�, ����������� 0,1��
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

