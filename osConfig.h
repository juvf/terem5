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
//#define FLAG_SLEEP_MESUR	( 1 << 1 )
#define FLAG_SLEEP_UART		( 1 << 2 )
#define FLAG_IS_READY_MES	( 1 << 3 )
#define FLAG_WRITE_PARAM	( 1 << 4 )
#define FLAG_COM_USB		( 1 << 5 )
#define FLAG_FLASH_CLEARING	( 1 << 6 )
#define FLAG_USB_NO_POWER		( 1 << 8 ) //флаг установлен, если НЕТ питания по USB
#define FLAG_USB_INIT		(1 << 9)//установлен, когда USB проинициализирован
#define FLAG_SLEEP_USB		(1 << 10)//установлен, когда нет соединения по усб и усб деинициализирован


//эти флаги стоят в 1 когда данная задача не работает и не требует активности от процессора
#define FLAG_NO_WORKS_USB			(1<<11)
#define FLAG_NO_WORKS_MESURING		(1<<12)
#define FLAG_NO_WORKS_WRITE_PARAM 	( 1 << 7 )
#define FLAG_NO_WORKS_BT			(1<<13)
#define FLAG_NO_WORK	(FLAG_NO_WORKS_BT | FLAG_NO_WORKS_USB | FLAG_NO_WORKS_WRITE_PARAM)



#define TASK_PRIORITY_MAIN		0
#define TASK_PRIORITY_MESUR		1
#define TASK_PRIORITY_RFD		1
#define TASK_PRIORITY_USB		2

#define SIZE_BUFF_WH41	128
  
extern xTaskHandle handleMain;
extern xTaskHandle handleMesur;
extern xTaskHandle handleUsb;
extern xTaskHandle handleRfd;

extern xTimerHandle timerClearFlash;

extern SemaphoreHandle_t semaphAdc; //симафор для измерений
extern SemaphoreHandle_t mutexFlash; //мьютикс для доступа к флешпамяти

extern xQueueHandle cansolQueue;
extern xQueueHandle uartRfd232Queue;
extern xQueueHandle memComUsbQueue;
//extern xQueueHandle wt41AQueue;
extern EventGroupHandle_t xEventGroup;

void initOs(void);
void createTasks(void);
void callbackClearFlash(xTimerHandle timer);

#ifdef __cplusplus
}
#endif

#endif
