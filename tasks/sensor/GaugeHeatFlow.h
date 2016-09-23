/*
 * GaugeHeatFlow.h
 *
 *  Created on: 07 окт. 2015 г.
 *      Author: anisimov.e
 */

#ifndef TASKS_SENSOR_GAUGEHEATFLOW_H_
#define TASKS_SENSOR_GAUGEHEATFLOW_H_


//Преобразование напряжения в тепловой поток ----------------------------------
float HF_Flow(float curU, float k);

//Преобразование напряжения в тепловой поток, по варианту ВНИИМС --------------
float HF_Flow48(float curU, unsigned char Channel);


#endif /* TASKS_SENSOR_GAUGEHEATFLOW_H_ */
