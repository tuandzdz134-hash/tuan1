/**
 * Main Program: 7-Segment Display with PIC16F877A and DS1307 RTC
 * Features:
 * - Display time from DS1307 on 4-digit 7-segment LED
 * - Push button controls: UP, DOWN, SET, RESET
 * - Time adjustment functionality
 * 
 * Button Functions:
 * - RB4 (UP): Increment selected time value
 * - RB5 (DOWN): Decrement selected time value
 * - RB6 (SET): Cycle through editable fields (Hours -> Minutes -> Display)
 * - RB7 (RESET): Reset to 12:00
 */

#include <pic.h>
#include "7segment_display.h"
#include "i2c_ds1307.h"
#include "ds1307_registers.h"

// Configuration bits
__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_ON & BOREN_ON & LVP_OFF);

// Function prototypes
void delay_ms(unsigned int ms);
void delay_us(unsigned int us);
void interrupt isr(void);
void button_init(void);
unsigned char button_read(unsigned char button_pin);
void process_buttons(void);
void edit_mode_handler(void);

// Global variables
volatile unsigned int timer0_counter = 0;
DS1307_Time current_time = {0, 0, 12, 1, 1, 1, 26};

// Button states
unsigned char button_state[4] = {0, 0, 0, 0};  // UP, DOWN, SET, RESET
unsigned char button_pressed[4] = {0, 0, 0, 0};
unsigned char button_held[4] = {0, 0, 0, 0};
unsigned char edit_mode = 0;  // 0=display, 1=hours, 2=minutes
unsigned char hold_counter[4] = {0, 0, 0, 0};

// Button pin definitions
#define BTN_UP    RB4
#define BTN_DOWN  RB5
#define BTN_SET   RB6
#define BTN_RESET RB7

/**
 * Main program entry point
 */
void main(void) {
    // Initialize peripherals
    display_init();
    ds1307_init();
    button_init();
    
    // Initialize Timer0 for display multiplexing and button debouncing
    OPTION_REG = 0x07;  // Timer0 prescaler 1:256
    TMR0 = 0;
    GIE = 1;            // Enable global interrupts
    T0IE = 1;           // Enable Timer0 interrupt
    
    // Main loop
    while (1) {
        // Read time from DS1307 every second
        ds1307_read_time(&current_time);
        
        // Process button inputs
        process_buttons();
        
        // Update display
        if (edit_mode == 0) {
            // Display mode: show time
            display_time(current_time.hours, current_time.minutes);
            display_set_decimal(1);  // Show decimal point between hours and minutes
        } else if (edit_mode == 1) {
            // Edit hours mode
            display_time(current_time.hours, current_time.minutes);
            display_clear_decimal(0);  // Blink decimal point on hours
        } else if (edit_mode == 2) {
            // Edit minutes mode
            display_time(current_time.hours, current_time.minutes);
            display_clear_decimal(2);  // Blink decimal point on minutes
        }
        
        delay_ms(100);
    }
}

/**
 * Initialize button inputs
 */
void button_init(void) {
    TRISB |= 0xF0;  // RB4-RB7 as inputs
}

/**
 * Read button state (0 = pressed, 1 = released for active-low)
 */
unsigned char button_read(unsigned char button_pin) {
    return (PORTB >> button_pin) & 0x01;
}

/**
 * Process button inputs with debouncing
 */
void process_buttons(void) {
    static unsigned char debounce_counter[4] = {0, 0, 0, 0};
    unsigned char i;
    unsigned char button_pins[4] = {4, 5, 6, 7};  // RB4, RB5, RB6, RB7
    
    for (i = 0; i < 4; i++) {
        unsigned char new_state = button_read(button_pins[i]);
        
        // Debouncing logic
        if (new_state == button_state[i]) {
            debounce_counter[i] = 0;
        } else {
            debounce_counter[i]++;
            if (debounce_counter[i] >= 3) {
                button_state[i] = new_state;
                debounce_counter[i] = 0;
                
                // Button press detected
                if (button_state[i] == 0) {
                    button_pressed[i] = 1;
                    hold_counter[i] = 0;
                }
                // Button release
                else {
                    button_pressed[i] = 0;
                    button_held[i] = 0;
                }
            }
        }
        
        // Long press detection
        if (button_pressed[i]) {
            hold_counter[i]++;
            if (hold_counter[i] > 50) {
                button_held[i] = 1;
            }
        }
    }
    
    // Handle button actions
    if (button_pressed[0] || button_held[0]) {
        // UP button
        edit_mode_handler();
        if (edit_mode == 1) {
            current_time.hours++;
            if (current_time.hours > 23) current_time.hours = 0;
            ds1307_write_time(&current_time);
        } else if (edit_mode == 2) {
            current_time.minutes++;
            if (current_time.minutes > 59) current_time.minutes = 0;
            ds1307_write_time(&current_time);
        }
    }
    
    if (button_pressed[1] || button_held[1]) {
        // DOWN button
        edit_mode_handler();
        if (edit_mode == 1) {
            if (current_time.hours == 0) current_time.hours = 23;
            else current_time.hours--;
            ds1307_write_time(&current_time);
        } else if (edit_mode == 2) {
            if (current_time.minutes == 0) current_time.minutes = 59;
            else current_time.minutes--;
            ds1307_write_time(&current_time);
        }
    }
    
    if (button_pressed[2]) {
        // SET button - cycle edit modes
        edit_mode++;
        if (edit_mode > 2) edit_mode = 0;
    }
    
    if (button_pressed[3]) {
        // RESET button - reset to 12:00
        current_time.hours = 12;
        current_time.minutes = 0;
        current_time.seconds = 0;
        ds1307_write_time(&current_time);
        edit_mode = 0;
    }
}

/**
 * Handle edit mode - allows editing without rapid changes
 */
void edit_mode_handler(void) {
    static unsigned char last_mode = 0;
    
    if (edit_mode != last_mode) {
        last_mode = edit_mode;
    }
}

/**
 * Timer0 Interrupt Service Routine
 * Handles display multiplexing (every ~5ms at 4MHz)
 */
void interrupt isr(void) {
    if (T0IF) {
        T0IF = 0;
        TMR0 = 131;  // Reload value for ~5ms interrupt
        
        // Refresh display (multiplex one digit)
        display_refresh();
        
        // Increment display refresh counter
        timer0_counter++;
    }
}

/**
 * Delay in milliseconds
 */
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);  // Adjusted for 4MHz oscillator
}

/**
 * Delay in microseconds
 */
void delay_us(unsigned int us) {
    unsigned int i;
    for (i = 0; i < us; i++);
}
