/**
 * DS1307 Register Definitions and Constants
 * Real-Time Clock Module
 */

#ifndef DS1307_REGISTERS_H
#define DS1307_REGISTERS_H

// DS1307 I2C Address
#define DS1307_ADDRESS 0x68

// DS1307 Register Addresses
#define DS1307_SECONDS   0x00
#define DS1307_MINUTES   0x01
#define DS1307_HOURS     0x02
#define DS1307_DAY       0x03
#define DS1307_DATE      0x04
#define DS1307_MONTH     0x05
#define DS1307_YEAR      0x06
#define DS1307_CONTROL   0x07

// Control Register Bits
#define DS1307_CH_BIT    0x80  // Clock Halt bit (bit 7 of Seconds register)
#define DS1307_12_24     0x40  // 12/24 Hour mode bit (bit 6 of Hours register)
#define DS1307_PM_BIT    0x20  // PM bit in 12-hour mode (bit 5 of Hours register)

// Data structure for time
typedef struct {
    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;
    unsigned char day;
    unsigned char date;
    unsigned char month;
    unsigned char year;
} DS1307_Time;

#endif
