#include "osConfig.h"
#include "tasks/mainTask.h"
#include "tasks/usbTask.h"
#include "tasks/taskUartRfd.h"
#include "stm32f4xx.h"
#include "tasks/taskMeasurement.h"

//дескрипторы задач
xTaskHandle handleMain;
xTaskHandle handleMesur;
xTaskHandle handleUsb;
xTaskHandle handleRF;

xTimerHandle timerMesuring;

EventGroupHandle_t xEventGroup;
SemaphoreHandle_t semaphAdc;

xQueueHandle cansolQueue;//очередь для сообщений дебага в кансоль
xQueueHandle uartRfd232Queue;

#define SIZE_STACK_MESUR (configMINIMAL_STACK_SIZE*2)
#define SIZE_STACK_MAIN (configMINIMAL_STACK_SIZE)
#define SIZE_STACK_USB (configMINIMAL_STACK_SIZE)
#define SIZE_STACK_RFD (configMINIMAL_STACK_SIZE * 2)

void initOs(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	cansolQueue = xQueueCreate(20, sizeof(char*));
	uartRfd232Queue = xQueueCreate(10, 1);

	xEventGroup = xEventGroupCreate();

	timerMesuring = xTimerCreate("mesuring", 1000, pdTRUE, 0, timerMeasurement);
	semaphAdc = xSemaphoreCreateMutex();

	createTasks();
}

void createTasks()
{
	BaseType_t pTask;
	pTask = xTaskCreate(mainTask, "mainTask", SIZE_STACK_MAIN, 0,
			TASK_PRIORITY_MAIN, &handleMain);
	pTask = xTaskCreate(taskMeasurement, "mesurTask", SIZE_STACK_MESUR, 0,
			TASK_PRIORITY_MESUR, &handleMesur);
//	pTask &= xTaskCreate(usbTask, "usbTask", SIZE_STACK_USB, 0,
//			TASK_PRIORITY_USB, &handleUsb);
	pTask &= xTaskCreate(taskUartRfd, "taskUartRfd", SIZE_STACK_RFD, 0,
			TASK_PRIORITY_RFD, &handleRF);

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
