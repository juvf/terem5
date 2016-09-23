/*
 * GaugeHeatFlow.h
 *
 *  Created on: 07 ���. 2015 �.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_GAUGEHEATFLOW_H_
#define TASKS_SENSOR_GAUGEHEATFLOW_H_


//�������������� ���������� � �������� ����� ----------------------------------
float HF_Flow(float curU, float k);

//�������������� ���������� � �������� �����, �� �������� ������ --------------
float HF_Flow48(float curU, unsigned char Channel);


#endif /* TASKS_SENSOR_GAUGEHEATFLOW_H_ */
