/*
 * commandsRfd.h
 *
 *  Created on: 16 ма€ 2015 г.
 *      Author: juvf
 */

#ifndef TASKS_COMMANDSRFD_H_
#define TASKS_COMMANDSRFD_H_

#include <stdint.h>

//команды принимают игаук с запросом, формируют ответ не трога€ заголовки кадра
//и возвращ€ют кол-во байти ответа без црц

int commandTestConnect(uint8_t *buffer);
int commandError(uint8_t *buffer);
int commandClearFlash(uint8_t *buffer);
int commandReadFlash(uint8_t *buffer);
int commandGetCurAdc(uint8_t *buffer);
int commandT48(uint8_t *buffer);
int commandGetState(uint8_t *buffer);
int commandReadyCheck(uint8_t *buffer);
int commandReadVersion(uint8_t *buffer);

#endif /* TASKS_COMMANDSRFD_H_ */
