/*
 ������ � ����������������� ��������� �����/����/��������
 ������������ ������������� ������������ a1_x, a0_x (�����/����/��������)

 ������ � ����������������� ��������� ������������� ����������
 ������������ ������������ A4-A0, B4-B0

 ��� ����������� ������������� ���������� dL/L=1.5% (15000 ���/�)

 ��� Ks=2
 1 ��� ��� ������������� 0.55 ��� (��� 1.1 ���/�)
 ��� R=200 �� ���� 100 ��� ������������� 0.4 �� ��� dR/R=0.2% ��� dL/L=0.1%
 ��� �������� dU=500.5 ��� ��� 1000 ���/� ��� 820 ��. ���

 ������������� ����� ��� ���������� r=Rg/(Ks*e), ��� Rg-����. �������,
 Ks-����. ���������������������, e=dL/L
 */

#include "GaugeTenso.h"
#include "../../structCommon.h"
#include "main.h"
#include <math.h>

extern float AB[10]; //������������ A4-A0, B4-B0
#define Ks     AB[0]   //����������� ���������������������
#define R      AB[1]   //������������� ������������
#define Kcal0  AB[2]   //����������� ���������� ����
#define KcalF  AB[3]   //����������� ���������� ������ �����
#define Mode *(BYTE*)&AB[4] //���� ������ ���������
//1-��������� - ���������� �����
//2-���������� ���� � ������ �����
//3-���������� ������

//�������� ���������� ������ (� ����)
#define R1  100000
#define R2   50000
//����� ��� ��������� ���������� �����, ��
#define RtrU   300
//����� ��� ��������� �������� �����, ��
#define RtrR   190

static void TenzROn(void);

//�������������� ���������� � �����/����/�������� -----------------------------
//���������� �������������� �������� � ��, ���������� �� ������������� �����������
float Tenso_1(float curU, float *a0)
{
	float *a1 = a0 - 1;
	return (curU - *a0) * *a1;
}

//�������������� ���������� � ������������� ���������� ------------------------
//����: ���������� ����������, ���. ����� ��������, ����� ������ ������������
//#pragma optimize=none
//float Tenso_2(float curU, BYTE Ad, BYTE Channel)
//{
//	float dRR, dLL;
//
//	//������ ������������� ������������ �� Flash
//	AB_Read(Ad, Channel, 0);
//	//�������� �� ������������
//	if( Ks==0) return flErrorCode;
//	if( R==0) return flErrorCode;
//	//����� ��������� ���� (����� ����������� ����������)
//	if( Mode == 1 )
//		return curU * 1000.0;
//	//��������� ����, ���������� �������������� ��������
//	else if( Mode == 2 )
//	{
//		BYTE CurRange = 0;
//		//���������� �������� ���������� ����
//		Kcal0=curU;
//
//		//��������� ������ �������� � ������
//		SetAdChannel(Ad);
//		if( DataFlash1.Flags & 1 )
//			SetChannel(Channel);
//		//��������� �����, ��������� ������� ���������� �����
//		TenzROn();
//		SetBrigeU();
//		IdleCPU(10);
//		//��������� ��������� ���������� ����������������� ����� � ������
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
//		//���������� �����
//		TenzROff();
//		//��������� dR/R (dR ������� ������������ ����� R1 ��� R2, R �������� �� ������������)
//		if( R>RtrR) dRR=-R/(R+R1);
//		else dRR=-R/(R+R2);
//		//��������� KcalF
//		curU -= Kcal0;
//		KcalF=dRR/curU;         // + = -/-
//		Mode = 1;                 //���������� ��������
//		//������ ���������� ������������� �� Flash
//		AB_Write(Ad, Channel, 1);
//		return Kcal0*1000.0;
//
//		//���������� ������������� ����������
//	}
//	else
//	{
//		curU-=Kcal0;  //���� ���������� ����
//		//��������� dR/R
//		dRR=curU*KcalF;
//		//��������� dL/L
//		dLL=dRR*1e6/Ks;
//		return dLL;
//	}
//}

//��������� ������� ��������� ����� -------------------------------------------
//static void TenzROn(void)
//{
//	if( R>RtrR) TenzR1On(); //������ ������ -> ������� �������� �����
//	else TenzR2On();//������� ������������� -> ��������
//}

////����� ���������� ���������� -------------------------------------------------
//void SetBrigeU(void)
//{
//	if( R>RtrU) TenzUHigh(); //������ ������ -> ������� ����������
//	else TenzUTiny();//������� ������������� -> ��������
//}

//������� ������������ ������������� ------------------------------------------
void DefaultKoefTenso(void)
{
}

#undef Ks
#undef R
#undef Kcal0
#undef KcalF
#undef Mode

/*
 ������ � ����������������� ��������� � ����������� ����������������
 */

//#define Full    AB[0]   //�������� ������ ����� � ���������� ��������
//#define Uref    AB[1]   //�������� ������ ����� � �������
//#define Sens    AB[2]   //����������������, ��/�
//#define U0      AB[3]   //�������� ����, ��
//#define Tcomp   AB[4]   //�����. ������������� �����������
//#define Type    *(GaugeTensoType*)&AB[5] //��� �������, GaugeTensoType
//#define T0      AB[6]   //����������� ��������� ����

//�������������� ���������� � ��������� ---------------------------------------
//����: ���������� ����������, ���. ����� ��������, ����� ������ ������������
float Tenso_3(float curU, uint8_t Channel, float T)
{
	float dLL, Res;

	//������ ������������� ������������ �� Flash
//	AB_Read(Ad, Channel, 1);

	float Full = koeffsAB.koef[Channel].a[0];//    AB[0]   //�������� ������ ����� � ���������� ��������
	float Uref = koeffsAB.koef[Channel].a[1];//   AB[1]   //�������� ������ ����� � �������
	float Sens = koeffsAB.koef[Channel].a[2];//   AB[2]   //����������������, ��/�
	float U0 = koeffsAB.koef[Channel].a[3];//     AB[3]   //�������� ����, ��
	float Tcomp = koeffsAB.koef[Channel].a[4];//  AB[4]   //�����. ������������� �����������
	float Type = *(GaugeTensoType*)&koeffsAB.koef[Channel].b[0];//   *(GaugeTensoType*)&AB[5] //��� �������, GaugeTensoType
	float T0 = koeffsAB.koef[Channel].b[1];//     AB[6]   //����������� ��������� ����

	//�������� �� ������������
	if( Full==0) return flErrorCode;
	if( !isnormal(Full)) return flErrorCode;
	if( Uref==0) return flErrorCode;
	if( !isnormal(Uref)) return flErrorCode;
	if( Sens==0) return flErrorCode;
	if( !isnormal(Sens)) return flErrorCode;

	//���� ��� ������ � �����������, �� ��������� ����������������
	if( !isnormal(T0) && (T0 != 0)) T0=25;
	if( T0== flErrorCode) T0=25;
	if( !isnormal(T) && (T != 0) )
		T = T0;
	if( T == flErrorCode )
		T = T0;

		//��������� ��������� �� ����� ��������
	switch(Type)
	{

		//KM-100B, ������������ ������ ���������� (strain gauge), Tokio Sokki Kenkyujo Co., Ltd.
		case GTT_KM100B:
			curU -= U0;                 //��������� �������� ����
			dLL=2e6*curU/Uref;// uE � ���/� (dR/R=4*dU/U, dL/L=dR/R/2)
			Res=dLL*Sens;// uE � ����������
			Res+=(Tcomp-9.3)*(T-T0);//����������������

			//uE3 = Ce * uEi + (Cbeta - gamma) * dT
			//                 Tcomp  9.3*10-6
			return Res;

			//����
			case GTT_Weigher:
			curU-=U0; curU *= 1000.0;//������� � ��, ��������� �������� ����
			Res=curU * Full / (Uref * Sens);
			return Res;

			//����������� ��� �������
			default:
			return flErrorCode;
		}
	}

////��������� ���� ����������������� ������� ------------------------------------
//void Tenso3SetZero(BYTE Ad, BYTE Chan)
//{
//	BYTE Comp;  //����� ������ ������������ �����������
//	float Temp; //�����������
//
//	//������ ������������� �� Flash
//	AB_Read(Ad, Chan, 1);
//	//������ �������� �������� ����
//	U0=U[Ad][Chan];     //�������� ����, ��
//
//	//����� ����������� ������������ � ������� �����������
//	Comp = *(Chan + &DataFlash1.DF_CompChan0);
//	Temp = Result[Ad][Comp];
//	if( !isnormal(Temp) && (Temp != 0) )
//		Temp = 0;
//	//������ ����������� ������������
//	T0=Temp;
//
//	//������ ������������� ������� �� Flash
//	AB_Write(Ad, Chan, 1);
//}

