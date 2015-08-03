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
  
#define TASK_PRIORITY_MAIN		0
#define TASK_PRIORITY_RFD		1
#define TASK_PRIORITY_USB		2
  
extern xTaskHandle handleMain;
extern xTaskHandle handleUsb;
extern xTaskHandle handleRfd;

extern xQueueHandle cansolQueue;
extern xQueueHandle uartRfd232Queue;



void initOs(void);
void createTasks(void);

#ifdef __cplusplus
}
#endif

#endif
