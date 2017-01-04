/*
 * taskUartRfd.cpp
 *
 *  Created on: 13 ��� 2015 �.
 *      Author: juvf
 */
#include "taskUartRfd.h"
#include "osConfig.h"
#include "stm32f4xx_conf.h"
#include "Checksum.h"
#include "commandsRfd.h"
#include "clock.h"
#include "Process.h"
#include "configTerem.h"
#include "main.h"

#include <string.h>
#include <stdio.h>

char *messSleep = "SLEEP\r\n";

#define BUFFER_SIZE	1024
#define ADRRESS		0x01
uint8_t rfd_buffer[4120];
uint16_t rfd_count;	//������� ��������/������������ ����
uint8_t rfd_addresSlave; //����� ���������� ������
uint8_t rfd_addresMaster; //����� ����������� ������
uint8_t rfd_sizeOfFrame; //������ ������
uint8_t rfd_IdFrame; //id ������
uint8_t rfd_command; //��� �������
uint16_t crc;
uint8_t fUart2Usb;
int endTransmit = 0;

//bool a23 = false;

//char replayWHh41[SIZE_BUFF_WH41];
int itWh41 = 0;

void taskUartRfd(void *context)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	vTaskDelay(1000); //�����, ��� ����, ����� ������ �� ����������� � �������� ��������� ����
	// �������� ���������� � ��������� USART
	USART_Cmd(USART2, ENABLE);
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_SendData(USART2, '\r');
	while((USART2->SR & (1 << 6)) == 0)
		;

	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_SendData(USART2, '\n');
	while((USART2->SR & (1 << 6)) == 0)
		; // ���� ������� ���������
	USART_ClearITPendingBit(USART2, USART_IT_TC);

//	USART_ClearITPendingBit(USART2, USART_IT_TC);
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);

	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 12);
	//������� ����� ����������

	sleepBt();

	xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_BT);

	rfd_count = 0;
	rfd_sizeOfFrame = 6;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ��� ���������
	uint8_t byte;

	for(;;)
	{
		rfd_count = 0;
		if( xQueueReceive(uartRfd232Queue, &byte, 10000) == pdTRUE )
		{
			reciveByte(byte);
			while(1)
			{
				if( xQueueReceive(uartRfd232Queue, &byte,
						100) == pdPASS )
				{
					if( reciveByte(byte) == false )
						break;
				}
				else
				{
					if( rfd_count >= 7 )
					{ //������ � ������ RFCOMM
						rfd_buffer[rfd_count] = 0;
						if( strstr((char*)rfd_buffer + 10, "RFCOMM") != NULL )
						{
							ledGreenOn();
							xEventGroupClearBits(xEventGroup, FLAG_NO_WORKS_BT);
						}
						else if( strncmp((char*)rfd_buffer, "NO CAR", 6) == 0 )
						{
							vTaskDelay(2);
							sleepBt();
						}
						else if( strstr((char*)rfd_buffer, messSleep) != NULL )
						{
							ledGreenOff();
							xEventGroupSetBits(xEventGroup, FLAG_NO_WORKS_BT);
						}
					}
					checkMsgForUsb();
					break;
				}
			}
			vTaskDelay(100);
		}
	}
}

void checkMsgForUsb()
{
	if( itWh41 > 0 )
	{
		fUart2Usb = 1;
	}
//	int size = SIZE_BUFF_WH41 - uxQueueSpacesAvailable(wt41AQueue);
//	if( size == 0 )
//		return;
//	else
//	{
//
//		xQueueReset(wt41AQueue);
//	}
}

bool reciveByte(uint8_t byte)
{
	rfd_buffer[rfd_count++] = byte;
	if( ++itWh41 >= BUFFER_SIZE )
		itWh41--;

	if( rfd_count >= BUFFER_SIZE )
		--rfd_count;
	if( rfd_count == 2 )
		rfd_sizeOfFrame = byte;

	if( (rfd_buffer[0] == 0x80) && (rfd_count > 4) )
	{
		if( rfd_count >= rfd_sizeOfFrame )
		{ //������� ���� �����
		  //��������� ���������� �� ������ ��������
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			if( Checksum::crc16(rfd_buffer, rfd_sizeOfFrame) == 0 )
			{
				parser(rfd_buffer, true);
			}
			else
				setRxMode();
			return false;
		}
	}
	return true;
}

void sleepBt()
{
	rfd_sizeOfFrame = strlen(messSleep);
	strcpy((char*)rfd_buffer, messSleep);

	rfd_count = 0;
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, ENABLE); // �� ��������� ��������
	USART_SendData(USART2, rfd_buffer[0]);
	vTaskDelay(500);

}

void setRxMode()
{
	rfd_count = 0;
	rfd_sizeOfFrame = 6;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
}

void parser(uint8_t *buf, uint8_t isRf)
{
	uint32_t asd = (uint32_t)buf;
	uint8_t sizeOfFrame = buf[1];
	uint8_t addresSlave = buf[2];
	uint8_t addresMaster = buf[3];
//	uint8_t	IdFrame = buf[4];
	uint8_t command = buf[5];
//
//		rfd_sizeOfFrame = buf[1];
//				rfd_addresSlave = buf[2];
//				rfd_addresMaster = buf[3];
//				rfd_IdFrame = buf[4];
//				rfd_command = buf[5];

	if( addresSlave != teremParam.address )
	{
		if( isRf )
			setRxMode();
		buf[1] = 0;
		return;
	}

	switch(command)
	{
		case UART_TestConnect:
			sizeOfFrame = commandTestConnect(buf);
			break;
		case UART_SetTime:
			if( setRtcTime(buf + 6) )
				sizeOfFrame = 6;
			else
				sizeOfFrame = commandError(buf);
			break;
		case UART_GetTime:
			getRtcTime(buf + 6);
			sizeOfFrame = 12;
			break;
		case 0x02: //�������� ���������� UART_ReadyCheck
			sizeOfFrame = commandReadyCheck(buf);
			break;
		case 0x07: //������ ������������
			sizeOfFrame = setConfigTerem(buf);
			break;
		case 0x06: //������ ������������
			sizeOfFrame = getConfigTerem(buf);
			break;
		case 0x17: //Start_Proc
			sizeOfFrame = commandStartProc(buf + 6);
			break;
		case 0x18: //Get_ProcConf ������ ����������
			sizeOfFrame = commandGetProcConf(buf + 6);
			break;
		case 0x19: //Get_CurrentVal
			sizeOfFrame = commandGetCurAdc(buf);
			break;
		case 0x21: //UART_ClearFlash
			sizeOfFrame = commandClearFlash(buf);
			break;
		case 0x23: //UART_GetCountProcess
			sizeOfFrame = commandGetCountProc(buf);
			break;
		case 0x24: //UART_GetHeaderProcess
			sizeOfFrame = commandGetHeaderProc(buf);
			break;
		case 0x22: //UART_ReadFlash
			sizeOfFrame = commandReadFlash(buf);
			break;
		case 0x09: //UART_Cmd48
			sizeOfFrame = commandT48(buf);
			break;
		case 0x20: //Stop_Proc+
			sizeOfFrame = commandStopProc();
			break;
		case UART_State:
			sizeOfFrame = commandGetState(buf);
			break;
		case UART_RemoveProcess: //������� �������
			sizeOfFrame = commandDeleteProc(buf);
			break;
		case UART_ReadVersion:
			sizeOfFrame = commandReadVersion(buf);
			break;
		default:
			sizeOfFrame = commandError(buf);
			break;
	}
	if( isRf )
	{ //����� �� ��
		if( rfd_sizeOfFrame > 0 )
		{
			buf[1] = sizeOfFrame + 2;
			buf[2] = addresMaster;
			buf[3] = ADRRESS;
			uint16_t crc = Checksum::crc16(buf, sizeOfFrame);
			buf[sizeOfFrame++] = (uint8_t)crc;
			buf[sizeOfFrame++] = (uint8_t)(crc >> 8);
			rfd_count = 0;
			rfd_sizeOfFrame = sizeOfFrame;
			USART_ClearITPendingBit(USART2, USART_IT_TC);
			USART_ITConfig(USART2, USART_IT_TC, ENABLE); // �� ��������� ��������
			USART_SendData(USART2, buf[0]);
		}
	}
	else
	{ //����� �� USB
		buf[1] = sizeOfFrame + 2;
		buf[2] = addresMaster;
		buf[3] = ADRRESS;
		uint16_t crc = Checksum::crc16(buf, sizeOfFrame);
		buf[sizeOfFrame++] = crc & 0xff;
		buf[sizeOfFrame] = (crc >> 8) & 0xff;
	}
}

//����� �� ��������
void replayWait(int msec)
{
	rfd_buffer[1] = 12;
	rfd_buffer[2] = rfd_buffer[3];
	rfd_buffer[3] = ADRRESS;

	uint16_t crc = Checksum::crc16(rfd_buffer, 10);
	rfd_buffer[10] = (uint8_t)crc;
	rfd_buffer[11] = (uint8_t)(crc >> 8);

	rfd_count = 0;
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ITConfig(USART2, USART_IT_TC, ENABLE); // �� ��������� ��������
	USART_SendData(USART2, rfd_buffer[0]);
}

void deinitUartRfd()
{

	NVIC_DisableIRQ(USART2_IRQn);
	USART_Cmd(USART2, DISABLE);

	rfd_count = 0;
	rfd_sizeOfFrame = 6;

	USART_DeInit(USART2);
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, DISABLE);

}

//������������ USART2
void initUartRfd()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	// ������������� ������ ����� �����������, ��� USART2 �
	// PA2 � PA3
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_OType = GPIO_OType_PP;	//TxD
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);

	// � ��� ��� �������, ������� �� ���� ��� ������ � STM32F10x,
	// �� ������� ����� �������� ��� ������������� STM32F4xx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// � ������ ����������� ������ USART
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_Init(USART2, &usart);

	rfd_count = 0;
	rfd_sizeOfFrame = 6;

	//USART_ITConfig(USART3, USART_IT_TC, ENABLE); // �� ��������� ��������
	//USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // ��� ���������
}

extern "C" void USART2_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
//	if( (USART2->SR & (1<<6)) != 0 ) // ���� ������� ���������
	if( USART_GetITStatus(USART2, USART_IT_TC) ) // ���� ������� ���������
	{
		// ������� ���� ����������, ���� ����� �� �������, ��� ����� ���������� ���������.
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		if( ++rfd_count >= rfd_sizeOfFrame )
		{ //�������� ���� �����
			endTransmit = 0;
			setRxMode();
		}
		else
		{
			USART_SendData(USART2, rfd_buffer[rfd_count]);
		}
	}

	if( (USART2->SR & (1 << 5)) != 0 ) // ���� ������ �������� (������� ������ �� ����)
	{
		// ���� ������� ���������� ������������� ���������� ������
		static uint8_t byte;
		byte = USART_ReceiveData(USART2);
		if( xQueueSendFromISR(uartRfd232Queue, &byte,
				&xHigherPriorityTaskWoken) != pdTRUE )
			xQueueSendFromISR(uartRfd232Queue, &byte,
					&xHigherPriorityTaskWoken);
	}
#ifdef DEBUG_DF
	if( ircUart != 0 )
	{
		uint32_t cr1 = USART2->CR1;
		uint32_t cr2 = USART2->CR2;
		uint32_t cr3 = USART2->CR3;
		uint32_t sr = USART2->SR;

		if( USART_GetITStatus(USART2, USART_IT_PE) == SET )
		{
			while(1)
			ledSeitch(12);
		}
		if( USART_GetITStatus(USART2, USART_IT_TXE) == SET )
		{
			while(1)
			ledSeitch(3);
		}
		if( USART_GetITStatus(USART2, USART_IT_ORE_RX) == SET )
		{
			while(1)
			ledSeitch(4);
		}
		if( USART_GetITStatus(USART2, USART_IT_IDLE) == SET )
		{
			while(1)
			ledSeitch(5);
		}
		if( USART_GetITStatus(USART2, USART_IT_LBD) == SET )
		{
			while(1)
			ledSeitch(6);
		}
		if( USART_GetITStatus(USART2, USART_IT_CTS) == SET )
		{
			while(1)
			ledSeitch(7);
		}
		if( USART_GetITStatus(USART2, USART_IT_ERR) == SET )
		{
			while(1)
			ledSeitch(8);
		}
		if( USART_GetITStatus(USART2, USART_IT_ORE_ER) == SET )
		{
			while(1)
			ledSeitch(9);
		}
		if( USART_GetITStatus(USART2, USART_IT_NE) == SET )
		{
			while(1)
			ledSeitch(10);
		}
		if( USART_GetITStatus(USART2, USART_IT_FE) == SET )
		{
			while(1)
			ledSeitch(11);
		}

	}
#endif

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken == pdTRUE);
}
#ifdef DEBUG_DF
void ledSeitch(int n)
{
	int i = 0;
	for(; i < n; i++)
	{
		ledRedOn();
		pauseT(1000);
		ledRedOff();
		pauseT(1000);
	}
	pauseT(5000);
}

void pauseT(int t)
{
	uint32_t i, j;
	for(j = 0; j < t; j++)
	for(i = 0; i < 700; i++)
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
#endif

