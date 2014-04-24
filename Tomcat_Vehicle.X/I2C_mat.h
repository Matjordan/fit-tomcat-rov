/* 
 * File:   I2C_mat.h
 * Author: Mathew
 *
 * Created on April 8, 2014, 3:51 PM
 */

#ifndef I2C_MAT_H
#define	I2C_MAT_H

#include <plib/i2c.h>

unsigned char I2CReadByte(unsigned char add,unsigned char subadd);
void I2CWriteByte(unsigned char add,unsigned char subadd,unsigned char data);

#endif	/* I2C_MAT_H */

