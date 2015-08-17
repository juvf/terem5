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
int commandSetConfig(uint8_t *buffer);
int commandGetConfig(uint8_t *buffer);
int commandClearFlash(uint8_t *buffer);
int commandReadFlash(uint32_t adrInFlash, uint8_t *distanation, uint16_t size);

#endif /* TASKS_COMMANDSRFD_H_ */
