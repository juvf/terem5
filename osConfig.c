#include "osConfig.h"
#include "tasks/mainTask.h"
#include "tasks/usbTask.h"
#include "tasks/taskUartRfd.h"
#include "stm32f4xx.h"
#include "tasks/taskMeasurement.h"

//дескрипторы задач
xTaskHandle handleMain;
xTaskHandle handleUsb;

xTimerHandle timerMesuring;

xQueueHandle cansolQueue;//очередь для сообщений дебага в кансоль
xQueueHandle uartRfd232Queue;

#define SIZE_STACK_MAIN (configMINIMAL_STACK_SIZE)
#define SIZE_STACK_USB (configMINIMAL_STACK_SIZE)
#define SIZE_STACK_RFD (configMINIMAL_STACK_SIZE)

void initOs(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	cansolQueue = xQueueCreate(20, sizeof(char*));
	uartRfd232Queue = xQueueCreate(10, 1);

	timerMesuring = xTimerCreate("mesuring", 1000, pdTRUE, 0, taskMeasurement);

	createTasks();
}

void createTasks()
{
	BaseType_t pTask;
	pTask = xTaskCreate(mainTask, "mainTask", SIZE_STACK_MAIN, 0,
			TASK_PRIORITY_MAIN, &handleMain);
	pTask &= xTaskCreate(usbTask, "usbTask", SIZE_STACK_USB, 0,
			TASK_PRIORITY_USB, &handleUsb);
	pTask &= xTaskCreate(taskUartRfd, "taskUartRfd", SIZE_STACK_RFD, 0,
			TASK_PRIORITY_RFD, &handleUsb);



	if(pTask != pdPASS)
		while(1)
			;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS()
	;
	for(;;)
		;
}

void vApplicationMallocFailedHook(void)
{
	taskDISABLE_INTERRUPTS()
	;
	for(;;)
		;
}
