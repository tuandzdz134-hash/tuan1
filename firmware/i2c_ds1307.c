/**
 * DS1307 RTC I2C Communication Implementation
 * PIC16F877A with DS1307
 * Uses Software I2C (Bit-banging)
 */

#include "i2c_ds1307.h"
#include <pic.h>

// I2C Pin Definitions
#define SCL RC3
#define SDA RC4

#define SCL_TRIS TRISC3
#define SDA_TRIS TRISC4

// I2C Timing Delay (adjust based on oscillator frequency)
#define I2C_DELAY() { __asm MOVLW 0x04 __asm MOVWF 0x21 __asm DECFSZ 0x21 __asm GOTO $-1 }

/**
 * Initialize I2C communication
 */
void i2c_init(void) {
    SCL_TRIS = 1;  // SCL as input (open-drain)
    SDA_TRIS = 1;  // SDA as input (open-drain)
    SCL = 0;
    SDA = 0;
}

/**
 * I2C Start Condition: SDA goes LOW while SCL is HIGH
 */
void i2c_start(void) {
    SDA_TRIS = 1;  // SDA = 1 (release)
    I2C_DELAY();
    SCL_TRIS = 1;  // SCL = 1 (release)
    I2C_DELAY();
    SDA_TRIS = 0;  // SDA = 0 (pull down)
    I2C_DELAY();
    SCL_TRIS = 0;  // SCL = 0 (pull down)
}

/**
 * I2C Stop Condition: SDA goes HIGH while SCL is HIGH
 */
void i2c_stop(void) {
    SDA_TRIS = 0;  // SDA = 0 (pull down)
    I2C_DELAY();
    SCL_TRIS = 1;  // SCL = 1 (release)
    I2C_DELAY();
    SDA_TRIS = 1;  // SDA = 1 (release)
    I2C_DELAY();
}

/**
 * Write one byte on I2C bus
 * MSB first, LSB last
 */
void i2c_write_byte(unsigned char data) {
    unsigned char i;
    
    for (i = 0; i < 8; i++) {
        if (data & 0x80)
            SDA_TRIS = 1;  // Send 1
        else
            SDA_TRIS = 0;  // Send 0
        
        I2C_DELAY();
        SCL_TRIS = 1;      // SCL = 1 (release)
        I2C_DELAY();
        I2C_DELAY();
        SCL_TRIS = 0;      // SCL = 0 (pull down)
        
        data <<= 1;
    }
    
    // Release SDA for ACK bit
    SDA_TRIS = 1;
    I2C_DELAY();
    SCL_TRIS = 1;         // SCL = 1 (release)
    I2C_DELAY();
    SCL_TRIS = 0;         // SCL = 0 (pull down)
}

/**
 * Read one byte from I2C bus
 * ack: 1 = send ACK, 0 = send NACK
 */
unsigned char i2c_read_byte(unsigned char ack) {
    unsigned char i, data = 0;
    
    SDA_TRIS = 1;  // Release SDA
    
    for (i = 0; i < 8; i++) {
        data <<= 1;
        I2C_DELAY();
        SCL_TRIS = 1;      // SCL = 1 (release)
        I2C_DELAY();
        I2C_DELAY();
        
        if (SDA)           // Read bit
            data |= 0x01;
        
        SCL_TRIS = 0;      // SCL = 0 (pull down)
    }
    
    // Send ACK or NACK
    if (ack)
        SDA_TRIS = 0;      // SDA = 0 (ACK)
    else
        SDA_TRIS = 1;      // SDA = 1 (NACK)
    
    I2C_DELAY();
    SCL_TRIS = 1;         // SCL = 1 (release)
    I2C_DELAY();
    SCL_TRIS = 0;         // SCL = 0 (pull down)
    SDA_TRIS = 1;         // Release SDA
    
    return data;
}

/**
 * Send ACK on I2C bus
 */
void i2c_ack(void) {
    SDA_TRIS = 0;  // SDA = 0
    I2C_DELAY();
    SCL_TRIS = 1;  // SCL = 1
    I2C_DELAY();
    SCL_TRIS = 0;  // SCL = 0
}

/**
 * Send NACK on I2C bus
 */
void i2c_nack(void) {
    SDA_TRIS = 1;  // SDA = 1
    I2C_DELAY();
    SCL_TRIS = 1;  // SCL = 1
    I2C_DELAY();
    SCL_TRIS = 0;  // SCL = 0
}

/**
 * Convert BCD to Decimal
 */
unsigned char bcd_to_dec(unsigned char bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/**
 * Convert Decimal to BCD
 */
unsigned char dec_to_bcd(unsigned char dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

/**
 * Initialize DS1307 RTC
 */
void ds1307_init(void) {
    i2c_init();
}

/**
 * Read time from DS1307
 */
void ds1307_read_time(DS1307_Time *time) {
    i2c_start();
    i2c_write_byte((DS1307_ADDRESS << 1) | 0);  // Address + Write
    i2c_write_byte(DS1307_SECONDS);              // Start from Seconds register
    
    i2c_start();
    i2c_write_byte((DS1307_ADDRESS << 1) | 1);  // Address + Read
    
    time->seconds = bcd_to_dec(i2c_read_byte(1));  // Read Seconds, send ACK
    time->minutes = bcd_to_dec(i2c_read_byte(1));  // Read Minutes, send ACK
    time->hours = bcd_to_dec(i2c_read_byte(1));    // Read Hours, send ACK
    time->day = bcd_to_dec(i2c_read_byte(1));      // Read Day, send ACK
    time->date = bcd_to_dec(i2c_read_byte(1));     // Read Date, send ACK
    time->month = bcd_to_dec(i2c_read_byte(1));    // Read Month, send ACK
    time->year = bcd_to_dec(i2c_read_byte(0));     // Read Year, send NACK
    
    i2c_stop();
}

/**
 * Write time to DS1307
 */
void ds1307_write_time(DS1307_Time *time) {
    i2c_start();
    i2c_write_byte((DS1307_ADDRESS << 1) | 0);  // Address + Write
    i2c_write_byte(DS1307_SECONDS);              // Start from Seconds register
    
    i2c_write_byte(dec_to_bcd(time->seconds));
    i2c_write_byte(dec_to_bcd(time->minutes));
    i2c_write_byte(dec_to_bcd(time->hours));
    i2c_write_byte(dec_to_bcd(time->day));
    i2c_write_byte(dec_to_bcd(time->date));
    i2c_write_byte(dec_to_bcd(time->month));
    i2c_write_byte(dec_to_bcd(time->year));
    
    i2c_stop();
}
