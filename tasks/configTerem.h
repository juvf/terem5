/*
 * configTerem.h
 *
 *  Created on: 05 рту. 2015 у.
 *      Author: juvf
 */

#include <stdint.h>
#include "structCommon.h"

extern TDataFlash configTerem;

void intiDefaultConfig();
void initConfigTerem();
int setConfigTerem(uint8_t *buffer);
int getConfigTerem(uint8_t *buffer);


