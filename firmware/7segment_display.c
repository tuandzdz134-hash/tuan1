/**
 * 7-Segment LED Display Driver
 * Common Anode Configuration with Multiplexing
 */

#include <pic.h>
#include "7segment_display.h"

// 7-Segment LED Codes (Common Anode) - inverted logic
// Bit order: GFEDCBA (DP=bit7, A=bit0)
const unsigned char segment_codes[10] = {
    0xC0,  // 0: 1100 0000 = 0xC0 (segments: A,B,C,D,E,F)
    0xF9,  // 1: 1111 1001 = 0xF9 (segments: B,C)
    0xA4,  // 2: 1010 0100 = 0xA4 (segments: A,B,D,E,G)
    0xB0,  // 3: 1011 0000 = 0xB0 (segments: A,B,C,D,G)
    0x99,  // 4: 1001 1001 = 0x99 (segments: B,C,F,G)
    0x92,  // 5: 1001 0010 = 0x92 (segments: A,C,D,F,G)
    0x82,  // 6: 1000 0010 = 0x82 (segments: A,C,D,E,F,G)
    0xF8,  // 7: 1111 1000 = 0xF8 (segments: A,B,C)
    0x80,  // 8: 1000 0000 = 0x80 (all segments)
    0x90,  // 9: 1001 0000 = 0x90 (segments: A,B,C,D,F,G)
};

// Digit positions
unsigned char display_buffer[4] = {0, 0, 0, 0};
unsigned char current_digit = 0;

/**
 * Initialize 7-segment display
 * Configure PORTA as output for segments
 * Configure PORTB as output for digit selection
 */
void display_init(void) {
    TRISA = 0x00;  // PORTA as output (segments)
    TRISB = 0x00;  // PORTB as output (digit select)
    
    // Turn off all segments
    PORTA = 0xFF;  // All segments OFF (common anode)
    
    // Turn off all digits
    PORTB = 0x0F;  // All digit pins HIGH (BC557 off)
}

/**
 * Display a single digit on 7-segment LED
 */
void display_digit(unsigned char digit, unsigned char position) {
    if (digit <= 9) {
        display_buffer[position] = segment_codes[digit];
    }
}

/**
 * Display a number on 4-digit display (HH:MM format)
 * hours: 0-23
 * minutes: 0-59
 */
void display_time(unsigned char hours, unsigned char minutes) {
    unsigned char h1, h2, m1, m2;
    
    h1 = hours / 10;      // Hours tens digit
    h2 = hours % 10;      // Hours ones digit
    m1 = minutes / 10;    // Minutes tens digit
    m2 = minutes % 10;    // Minutes ones digit
    
    display_buffer[0] = segment_codes[h1];
    display_buffer[1] = segment_codes[h2];
    display_buffer[2] = segment_codes[m1];
    display_buffer[3] = segment_codes[m2];
}

/**
 * Display decimal number with leading zeros
 */
void display_number(unsigned int number) {
    unsigned char d1, d2, d3, d4;
    
    number = number % 10000;  // Limit to 4 digits
    
    d1 = number / 1000;
    d2 = (number / 100) % 10;
    d3 = (number / 10) % 10;
    d4 = number % 10;
    
    display_buffer[0] = segment_codes[d1];
    display_buffer[1] = segment_codes[d2];
    display_buffer[2] = segment_codes[d3];
    display_buffer[3] = segment_codes[d4];
}

/**
 * Multiplexing routine - call in interrupt or main loop
 * Displays one digit at a time
 */
void display_refresh(void) {
    // Turn off current digit
    PORTB |= 0x0F;  // All digits OFF
    
    // Move to next digit
    current_digit++;
    if (current_digit >= 4) {
        current_digit = 0;
    }
    
    // Set segments for current digit
    PORTA = display_buffer[current_digit];
    
    // Select current digit (active LOW for BC557)
    PORTB = ~(1 << current_digit);
}

/**
 * Display decimal point on specific digit
 */
void display_set_decimal(unsigned char position) {
    if (position < 4) {
        display_buffer[position] &= 0x7F;  // Clear bit 7 for decimal point
    }
}

/**
 * Clear decimal point on specific digit
 */
void display_clear_decimal(unsigned char position) {
    if (position < 4) {
        display_buffer[position] |= 0x80;  // Set bit 7
    }
}

/**
 * Clear all segments on specific digit
 */
void display_clear_digit(unsigned char position) {
    if (position < 4) {
        display_buffer[position] = 0xFF;  // All segments OFF
    }
}

/**
 * Clear entire display
 */
void display_clear_all(void) {
    unsigned char i;
    for (i = 0; i < 4; i++) {
        display_buffer[i] = 0xFF;  // All OFF
    }
}
