/*
 * Dpg2.h
 *
 *  Created on: 26 ����. 2016 �.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_DPG2_H_
#define TASKS_SENSOR_DPG2_H_

#include <stdint.h>
#include "../structCommon.h"

void dpg2_readValue(uint8_t numChanel, ResultMes *result);
void dpg2_readValue8(ResultMes *result);//�������� �� �������������� 9-�� ������
void init_Dtg2();
void init_Dtg28();
uint32_t readDtg();
uint32_t readDtg8();

#endif /* TASKS_SENSOR_DPG2_H_ */
