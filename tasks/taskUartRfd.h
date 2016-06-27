/*
 * taskUartRfd.h
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: juvf
 */

#ifndef TASKS_TASKUARTRFD_H_
#define TASKS_TASKUARTRFD_H_
#include <stdint.h> //9227520522

#define UART_MeasNow 	0x01 	//������� ������� �����
#define UART_ReadyCheck 0�02 	//������� �������� ����������
#define UART_RxData 	0x03	//������� ������ ����������� ���������
#define UART_RxConfig 	0�05	//������� ������ ������������ ��������
#define UART_RxFullConfig 	0�06//������� ������ ������ ������������ ��������
#define UART_TxConfig	0�07	//������� ������ ������������
#define UART_ZeroSetting	0�08//������� ��������� ����
#define UART_Cmd48		0�09	//����������� ������� ������ � 48-��������� ���������
#define UART_TestConnect 0x0C	//���� ����� � ���������
#define UART_Busy		0�0D	//����� - ���������� ������
#define UART_Error 		0x0E	//����� ������������ ������, �� �������������� �������
#define UART_Sleep		0x0F	//������� ���������������� ������ ������ ����� � ������
#define UART_SleepTillMeas 0x10	//������� ���������������� ������ ��������� � ������
#define UART_StartProc	0x11	//������� - ����� �������� ���������
#define UART_Synch		0x12	//������� ������������� ������� ������� �����
#define	UART_State		0x13	//������� ������ ��������� ��������
#define UART_DontSleep	0x14
#define	UART_SetTime	0x15	//������� ��������� ���� � �������
#define	UART_GetTime	0x16	//������� ������ ���� � �������
#define UART_RemoveProcess 0x25	//������� �������� ��������

enum { RING, NO_CARRIER, COMMAND };

#ifdef __cplusplus

void initUartRfd();
void deinitUartRfd();
bool reciveByte(uint8_t byte);
void parser();
void setRxMode();
void checkMsgForUsb();

extern "C"
{
#endif

void taskUartRfd(void *context);

#ifdef __cplusplus
}
#endif



#endif /* TASKS_TASKUARTRFD_H_ */
