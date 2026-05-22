// DS1307.h - I2C real time clock library for PIC
// Author: Generated for tuandzdz134-hash/tuan1 clock project
#ifndef _DS1307_H_
#define _DS1307_H_

#include <xc.h>

void I2C_Initialize(void);
unsigned char DS1307_Read(unsigned char address);
void DS1307_Write(unsigned char address, unsigned char data);
void DS1307_Init(void);

#endif
