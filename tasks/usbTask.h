/*
 * usbTask.h
 *
 *  Created on: 29 ���. 2015 �.
 *      Author: juvf
 */

#ifndef TASKS_USBTASK_H_
#define TASKS_USBTASK_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

extern uint8_t usbBuffer[280];

void usbTask(void *context);

#ifdef __cplusplus
}
#endif



#endif /* TASKS_USBTASK_H_ */
