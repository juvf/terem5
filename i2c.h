/*
 * i2c.h
 *
 *  Created on: 04 ���. 2015 �.
 *      Author: juvf
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>


void init_I2C1();
void i2cWrite(int slaveAdr, int address, uint8_t *buffer, int size);
void i2cRead(int slaveAdr, int address, uint8_t *buffer, int size);


#endif /* I2C_H_ */
