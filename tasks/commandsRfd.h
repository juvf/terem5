/*
 * commandsRfd.h
 *
 *  Created on: 16 ��� 2015 �.
 *      Author: juvf
 */

#ifndef TASKS_COMMANDSRFD_H_
#define TASKS_COMMANDSRFD_H_

#include <stdint.h>

//������� ��������� ����� � ��������, ��������� ����� �� ������ ��������� �����
//� ���������� ���-�� ����� ������ ��� ���

int commandTestConnect(uint8_t *buffer);
int commandError(uint8_t *buffer);
int commandClearFlash(uint8_t *buffer);
int commandReadFlash(uint8_t *buffer);
int commandGetCurAdc(uint8_t *buffer);
int commandT48(uint8_t *buffer);

#endif /* TASKS_COMMANDSRFD_H_ */
