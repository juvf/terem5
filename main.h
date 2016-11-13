/*
 * main.h
 *
 *  Created on: 27 ���. 2015 �.
 *      Author: juvf
 */

#ifndef MAIN_H_
#define MAIN_H_

#define ledGreenOn()	GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define ledGreenOff()	GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define ledRedOn()		GPIO_SetBits(GPIOA, GPIO_Pin_10)
#define ledRedOff()		GPIO_ResetBits(GPIOA, GPIO_Pin_10)


void pereferInit();
void pereferDeInit();
void deinitGPIO();
int main();
void initPa9();

#define flErrorCode   1e38


#endif /* MAIN_H_ */
