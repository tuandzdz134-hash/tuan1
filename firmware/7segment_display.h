/**
 * 7-Segment LED Display Header
 * Common Anode Configuration
 */

#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

// Function Prototypes
void display_init(void);
void display_digit(unsigned char digit, unsigned char position);
void display_time(unsigned char hours, unsigned char minutes);
void display_number(unsigned int number);
void display_refresh(void);
void display_set_decimal(unsigned char position);
void display_clear_decimal(unsigned char position);
void display_clear_digit(unsigned char position);
void display_clear_all(void);

#endif
