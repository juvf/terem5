/*
 * i2c.h
 *
 *  Created on: 04 рту. 2015 у.
 *      Author: juvf
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>


void init_I2C1(int u);
void i2cWrite(int slaveAdr, int address, uint8_t *buffer, int size);
void i2cWritePage(int slaveAdr, int address, uint8_t *buffer, int size);
void i2cRead(int slaveAdr, int address, uint8_t *buffer, int size);
void deinit_I2C1();


#endif /* I2C_H_ */
