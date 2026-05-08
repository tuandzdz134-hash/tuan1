/**
 * DS1307 RTC I2C Communication Header
 * PIC16F877A with DS1307
 */

#ifndef I2C_DS1307_H
#define I2C_DS1307_H

#include "ds1307_registers.h"

// Function Prototypes
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write_byte(unsigned char data);
unsigned char i2c_read_byte(unsigned char ack);
void i2c_ack(void);
void i2c_nack(void);

// DS1307 Specific Functions
void ds1307_write_time(DS1307_Time *time);
void ds1307_read_time(DS1307_Time *time);
void ds1307_init(void);

// BCD Conversion Functions
unsigned char bcd_to_dec(unsigned char bcd);
unsigned char dec_to_bcd(unsigned char dec);

#endif
