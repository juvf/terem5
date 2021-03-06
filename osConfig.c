#include "osConfig.h"
#include "structCommon.h"
#include "stm32f4xx.h"

#include "tasks/mainTask.h"
#include "tasks/usbTask.h"
#include "tasks/taskUartRfd.h"
#include "tasks/taskMeasurement.h"
#include "tasks/taskPowerSwitch.h"

//����������� �����
xTaskHandle handleMain;
xTaskHandle handleMesur;
xTaskHandle handleUsb;
xTaskHandle handleRF;
xTaskHandle handlePwr;

xTimerHandle timerClearFlash;

EventGroupHandle_t xEventGroup;
SemaphoreHandle_t semaphAdc;
SemaphoreHandle_t semaphInAdc; //������� ��� ������� � ����������� ���
SemaphoreHandle_t mutexFlash;
SemaphoreHandle_t mutexDeleteProc;

xQueueHandle cansolQueue;//������� ��� ��������� ������ � �������
xQueueHandle uartRfd232Queue;
xQueueHandle memComUsbQueue; //������� ��� ������ ������ �� ���
//xQueueHandle wt41AQueue;

#define SIZE_STACK_MESUR (configMINIMAL_STACK_SIZE*2)
#define SIZE_STACK_MAIN (configMINIMAL_STACK_SIZE * 2)
#define SIZE_STACK_USB (configMINIMAL_STACK_SIZE * 2)
#define SIZE_STACK_RFD (configMINIMAL_STACK_SIZE * 2)
#define SIZE_STACK_PWR (configMINIMAL_STACK_SIZE)

void initOs(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	cansolQueue = xQueueCreate(20, sizeof(char*));
	uartRfd232Queue = xQueueCreate(20, 1);
	memComUsbQueue = xQueueCreate(3, sizeof(MemCom));
	//wt41AQueue = xQueueCreate(SIZE_BUFF_WH41, 1);

	xEventGroup = xEventGroupCreate();

	timerClearFlash = xTimerCreate("clearFlash", 22000, pdFALSE, 0, callbackClearFlash);

	semaphAdc = xSemaphoreCreateMutex();
	semaphInAdc = xSemaphoreCreateMutex();
	mutexFlash = xSemaphoreCreateMutex();
	mutexDeleteProc = xSemaphoreCreateMutex();

	createTasks();
}

void createTasks()
{
	BaseType_t pTask;
	pTask = xTaskCreate(mainTask, "mainTask", SIZE_STACK_MAIN, 0,
			TASK_PRIORITY_MAIN, &handleMain);
	pTask = xTaskCreate(taskMeasurement, "mesurTask", SIZE_STACK_MESUR, 0,
			TASK_PRIORITY_MESUR, &handleMesur);
	pTask &= xTaskCreate(usbTask, "usbTask", SIZE_STACK_USB, 0,
			TASK_PRIORITY_USB, &handleUsb);
	pTask &= xTaskCreate(taskUartRfd, "taskUartRfd", SIZE_STACK_RFD, 0,
			TASK_PRIORITY_RFD, &handleRF);

	pTask &= xTaskCreate(taskPowerSwitch, "taskPwr", SIZE_STACK_PWR, 0,
			TASK_PRIORITY_PWR, &handleRF);

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

void vApplicationIdleHook(void)
{
	while(1)
	{
		__WFI();//Sleep
	}
}

void callbackClearFlash(xTimerHandle timer)
{
	xEventGroupClearBits(xEventGroup, FLAG_FLASH_CLEARING);
}
