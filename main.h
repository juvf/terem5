/*
 * main.h
 *
 *  Created on: 27 апр. 2015 г.
 *      Author: juvf
 */

#ifndef MAIN_H_
#define MAIN_H_

#define ledGreenOn()	GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define ledGreenOff()	GPIO_ClearBits(GPIOA, GPIO_Pin_9)
#define ledRedOn()		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
#define ledRedOff()		GPIO_ResetBits(GPIOA, GPIO_Pin_10);

void pereferInit();
void pereferDeInit();
int main();
void initUartForConsol();

#define flErrorCode   1e38


#endif /* MAIN_H_ */
