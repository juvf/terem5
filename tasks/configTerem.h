/*
 * configTerem.h
 *
 *  Created on: 05 ���. 2015 �.
 *      Author: juvf
 */

#include <stdint.h>
#include "structCommon.h"

extern TeremConfig configTerem;

void intiDefaultConfig();
void initConfigTerem();
void intiDefaultKoefAB();

int setConfigTerem(uint8_t *buffer);
int getConfigTerem(uint8_t *buffer);


