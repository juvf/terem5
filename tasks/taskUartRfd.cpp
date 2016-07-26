/*
 * taskUartRfd.cpp
 *
 *  Created on: 13 мая 2015 г.
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

#define BUFFER_SIZE	1024
#define ADRRESS		0x01
uint8_t rfd_buffer[4120];
uint16_t rfd_count;	//счетчик принятых/отправленных байт
uint8_t rfd_addresSlave; //адрес получателя пакета
uint8_t rfd_addresMaster; //адрес отправителя пакета
uint8_t rfd_sizeOfFrame; //длинна пакета
uint8_t rfd_IdFrame; //id пакета
uint8_t rfd_command; //код команды
uint16_t crc;
uint8_t fUart2Usb;
bool rfd_isReadReady;
int endTransmit = 0;

//char replayWHh41[SIZE_BUFF_WH41];
int itWh41 = 0;

void taskUartRfd(void *context)
{
	rfd_isReadReady = false;
	rfd_count = 0;
	rfd_sizeOfFrame = 6;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // При получении
	uint8_t byte;

//	//запретить прерывания по приему компорта
//	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
//	strcpy((char*)rfd_buffer, "SET BT AUTH * 1234\r\n");
//	rfd_sizeOfFrame = 20;
//	rfd_count = 0;
//	USART_ClearITPendingBit(USART2, USART_IT_TC);
//	USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
//	USART_SendData(USART2, rfd_buffer[0]);

	for(;;)
	{
		if( xQueueReceive(uartRfd232Queue, &byte, 15000) == pdTRUE )
		{
			if( 1 ) //byte == 0x80 )
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
						checkMsgForUsb();
						setRxMode();
						break;
					}
				}
			}
			vTaskDelay(100);
		}
		else
			xEventGroupSetBits(xEventGroup, FLAG_SLEEP_UART);
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
	static int flagRing;
	rfd_buffer[rfd_count++] = byte;
	if( ++itWh41 >= BUFFER_SIZE )
		itWh41--;

	if( rfd_count >= BUFFER_SIZE )
		--rfd_count;
	if( rfd_count == 2 )
		rfd_sizeOfFrame = byte;
	if( rfd_count == 4 )
	{
		if( strncmp((char*)rfd_buffer, "RING", 4) == 0 )
		{
			flagRing = RING;
			ledGreenOn();
		}
		else if( strncmp((char*)rfd_buffer, "NO C", 4) == 0 )
		{
			flagRing = NO_CARRIER;
			ledGreenOff();
		}
		else if( rfd_buffer[0] == 0x80 )
			flagRing = COMMAND;
	}

	if( flagRing == COMMAND )
	{
		if( rfd_count >= rfd_sizeOfFrame )
		{ //приняли весь пакет
		  //запретить прерывания по приему компорта
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			//rfd_isReadReady = true;
			if( Checksum::crc16(rfd_buffer, rfd_sizeOfFrame) == 0 )
			{
				parser();
			}
			else
				setRxMode();
			return false;
		}
	}
	return true;
}

void setRxMode()
{
	rfd_count = 0;
	rfd_sizeOfFrame = 6;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
}

void parser()
{
	rfd_sizeOfFrame = rfd_buffer[1];
	rfd_addresSlave = rfd_buffer[2];
	rfd_addresMaster = rfd_buffer[3];
	rfd_IdFrame = rfd_buffer[4];
	rfd_command = rfd_buffer[5];
	if( rfd_addresSlave != ADRRESS )
	{
		setRxMode();
		return;
	}
	switch(rfd_command)
	{
		case UART_TestConnect:
			rfd_sizeOfFrame = commandTestConnect(rfd_buffer);
			break;
		case UART_SetTime:
			if( setRtcTime(rfd_buffer + 6) )
				rfd_sizeOfFrame = 6;
			else
				rfd_sizeOfFrame = commandError(rfd_buffer);
			break;
		case UART_GetTime:
			getRtcTime(rfd_buffer + 6);
			rfd_sizeOfFrame = 12;
			break;
		case 0x02: //контроль готовности UART_ReadyCheck
			rfd_sizeOfFrame = commandReadyCheck(rfd_buffer);
			break;
		case 0x07: //запись конфигурации
			rfd_sizeOfFrame = setConfigTerem(rfd_buffer);
			break;
		case 0x06: //чтение конфигурации
			rfd_sizeOfFrame = getConfigTerem(rfd_buffer);
			break;
		case 0x17: //Start_Proc
			rfd_sizeOfFrame = commandStartProc(rfd_buffer + 6);
			break;
		case 0x18: //Get_ProcConf
			rfd_sizeOfFrame = commandGetProcConf(rfd_buffer + 6);
			break;
		case 0x19: //Get_CurrentVal
			rfd_sizeOfFrame = commandGetCurAdc(rfd_buffer);
			break;
		case 0x21: //UART_ClearFlash
			rfd_sizeOfFrame = commandClearFlash(rfd_buffer);
			break;
		case 0x23: //UART_GetCountProcess
			rfd_sizeOfFrame = commandGetCountProc(rfd_buffer);
			break;
		case 0x24: //UART_GetHeaderProcess
			rfd_sizeOfFrame = commandGetHeaderProc(rfd_buffer);
			break;
		case 0x22: //UART_ReadFlash
			rfd_sizeOfFrame = commandReadFlash(rfd_buffer);
			break;
		case 0x09: //UART_Cmd48
			rfd_sizeOfFrame = commandT48(rfd_buffer);
			break;
		case 0x20: //Stop_Proc+
			rfd_sizeOfFrame = commandStopProc();
			break;
		case UART_State:
			rfd_sizeOfFrame = commandGetState(rfd_buffer);
			break;
		case UART_RemoveProcess: //удалить процесс
			rfd_sizeOfFrame = commandDeleteProc(rfd_buffer);
			break;
		default:
			rfd_sizeOfFrame = commandError(rfd_buffer);
			break;
	}
	if( rfd_sizeOfFrame > 0 )
	{
		rfd_buffer[1] = rfd_sizeOfFrame + 2;
		rfd_buffer[2] = rfd_buffer[3];
		rfd_buffer[3] = ADRRESS;
		uint16_t crc = Checksum::crc16(rfd_buffer, rfd_sizeOfFrame);
		rfd_buffer[rfd_sizeOfFrame++] = (uint8_t)crc;
		rfd_buffer[rfd_sizeOfFrame++] = (uint8_t)(crc >> 8);

		rfd_count = 0;
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
		USART_SendData(USART2, rfd_buffer[0]);
	}
}

//ответ на ожидание
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
	USART_ITConfig(USART2, USART_IT_TC, ENABLE); // По окончанию отправки
	USART_SendData(USART2, rfd_buffer[0]);
}

void deinitUartRfd()
{
	USART_DeInit(USART2);
}

//задействован USART2
void initUartRfd()
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN, ENABLE);
	// Инициализация нужных пинов контроллера, для USART2 –
	// PA2 и PA3
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_OType = GPIO_OType_PP;	//TxD
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpio);

	// И вот еще функция, которой не было при работе с STM32F10x,
	// но которую нужно вызывать при использовании STM32F4xx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// А теперь настраиваем модуль USART
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;
	usart.USART_WordLength = USART_WordLength_8b;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_Init(USART2, &usart);
	// Включаем прерывания и запускаем USART
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 12);
	USART_Cmd(USART2, ENABLE);

	//USART_ITConfig(USART3, USART_IT_TC, ENABLE); // По окончанию отправки
	//USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // При получении
}

extern "C" void USART2_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	if( USART_GetITStatus(USART2, USART_IT_TC) != RESET ) // Если отпавка завершена
	{
		// Очищаем флаг прерывания, если этого не сделать, оно будет вызываться постоянно.
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		if( ++rfd_count == rfd_sizeOfFrame )
		{ //передали весь пакет
			endTransmit = 0;
			setRxMode();
		}
		else
		{
			USART_SendData(USART2, rfd_buffer[rfd_count]);
		}
	}

	if( USART_GetITStatus(USART2, USART_IT_RXNE) != RESET ) // Если приием завершен (регистр приема не пуст)
	{
		// Флаг данного прерывания сбрасыывается прочтением данных
		static uint8_t byte;
		byte = USART_ReceiveData(USART2);
		if( xQueueSendFromISR(uartRfd232Queue, &byte,
				&xHigherPriorityTaskWoken) != pdTRUE )
			xQueueSendFromISR(uartRfd232Queue, &byte,
					&xHigherPriorityTaskWoken);
		//xQueueSendFromISR(wt41AQueue, &byte, &xHigherPriorityTaskWoken);
		//replayWHh41[0] = 'f';
//		replayWHh41[itWh41] = byte;
//		if(replayWHh41[itWh41] == '\n')
//		{
//			replayWHh41[itWh41 + 1] = 0;
//			fUart2Usb = 1;
//			//xEventGroupSetBitsFromISR(xEventGroup, FLAG_UART_USB, &xHigherPriorityTaskWoken);
//			//itWh41 = 0;
//		}
//		else if(++itWh41 == (SIZE_BUFF_WH41 - 1))
//		{
//			replayWHh41[itWh41] = 0;
//			fUart2Usb = 1;
//			//xEventGroupSetBitsFromISR(xEventGroup, FLAG_UART_USB, &xHigherPriorityTaskWoken);
//			itWh41 = 0;
//		}
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken == pdTRUE);
}

