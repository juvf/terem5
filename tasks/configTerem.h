/*
 * configTerem.h
 *
 *  Created on: 05 ���. 2015 �.
 *      Author: juvf
 */

#include <stdint.h>
#include "structCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern TeremConfig configTerem;

void intiDefaultConfig();
void initConfigTerem();
void intiDefaultKoefAB();
void saveConfig();
void saveKoeffAB();
void saveParam();

int setConfigTerem(uint8_t *buffer);
int getConfigTerem(uint8_t *buffer);



#ifdef __cplusplus
}
#endif


