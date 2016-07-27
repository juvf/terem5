#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

#define FLAG_MESUR			( 1 << 0 )
#define FLAG_SLEEP_MESUR	( 1 << 1 )
#define FLAG_SLEEP_UART		( 1 << 2 )
#define FLAG_IS_READY_MES	( 1 << 3 )
#define FLAG_WRITE_PARAM	( 1 << 4 )
  
#define TASK_PRIORITY_MAIN		0
#define TASK_PRIORITY_MESUR		1
#define TASK_PRIORITY_RFD		2
#define TASK_PRIORITY_USB		3

#define SIZE_BUFF_WH41	128
  
extern xTaskHandle handleMain;
extern xTaskHandle handleMesur;
extern xTaskHandle handleUsb;
extern xTaskHandle handleRfd;

extern xTimerHandle timerMesuring;

extern SemaphoreHandle_t semaphAdc; //������� ��� ���������
extern SemaphoreHandle_t mutexFlash; //������� ��� ������� � ����������

extern xQueueHandle cansolQueue;
extern xQueueHandle uartRfd232Queue;
extern xQueueHandle memComUsbQueue;
//extern xQueueHandle wt41AQueue;
extern EventGroupHandle_t xEventGroup;



void initOs(void);
void createTasks(void);

#ifdef __cplusplus
}
#endif

#endif
