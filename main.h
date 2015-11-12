/*
 * main.h
 *
 *  Created on: 27 ���. 2015 �.
 *      Author: juvf
 */

#ifndef MAIN_H_
#define MAIN_H_

#define ledGreenOn()	GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define ledGreenOff()	GPIO_ResetBits(GPIOA, GPIO_Pin_9)
#define ledRedOn()		GPIO_SetBits(GPIOA, GPIO_Pin_10);
#define ledRedOff()		GPIO_ResetBits(GPIOA, GPIO_Pin_10);

void pereferInit();
void pereferDeInit();
void deinitGPIO();
int main();
void initUartForConsol();

#define flErrorCode   1e38


#endif /* MAIN_H_ */
