/*
 * taskUartRfd.h
 *
 *  Created on: 13 мая 2015 г.
 *      Author: juvf
 */

#ifndef TASKS_TASKUARTRFD_H_
#define TASKS_TASKUARTRFD_H_
#include <stdint.h> //9227520522

#define UART_MeasNow 	0x01 	//Команда сделать замер
#define UART_ReadyCheck 0х02 	//Команда контроль готовности
#define UART_RxData 	0x03	//Команда чтение результатов измерения
#define UART_RxConfig 	0х05	//Команда чтение конфигурации адаптера
#define UART_RxFullConfig 	0х06//Команда чтение полной конфигурации адаптера
#define UART_TxConfig	0х07	//Команда запись конфигурации
#define UART_ZeroSetting	0х08//Команда установка нуля
#define UART_Cmd48		0х09	//Расширенная команда работы с 48-канальным адаптером
#define UART_TestConnect 0x0C	//Тест связи с адаптером
#define UART_Busy		0х0D	//Ответ - устройство занято
#define UART_Error 		0x0E	//Ответ неправильный запрос, не поддерживаемая функция
#define UART_Sleep		0x0F	//Команда синхронизировать таймер сеанса связи и уснуть
#define UART_SleepTillMeas 0x10	//Команда синхронизировать таймер измерений и уснуть
#define UART_StartProc	0x11	//Команда - старт процесса измерения
#define UART_Synch		0x12	//Команда синхронизации таймера сеансов связи
#define	UART_State		0x13	//Команда чтения состояния адаптера
#define UART_DontSleep	0x14
#define	UART_SetTime	0x15	//Команда установки даты и времени
#define	UART_GetTime	0x16	//Команда чтения даты и времени
#define UART_RemoveProcess 0x25	//команда удаления процесса
#define UART_ReadVersion	0x27

enum { RING, NO_CARRIER, COMMAND };

#ifdef __cplusplus

void initUartRfd();
void deinitUartRfd();
bool reciveByte(uint8_t byte);
void parser(uint8_t *buf, uint8_t isRf);
void setRxMode();
void checkMsgForUsb();
void replayWait(int sec);

extern "C"
{
#endif

void taskUartRfd(void *context);

#ifdef __cplusplus
}
#endif



#endif /* TASKS_TASKUARTRFD_H_ */
