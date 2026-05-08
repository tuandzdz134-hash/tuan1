# 7-Segment LED Circuit with PIC16F877A and DS1307

## Project Overview
This project implements a 4-digit 7-segment LED display controlled by a PIC16F877A microcontroller with DS1307 RTC (Real-Time Clock) for timekeeping functionality.

## Components Required
- **Microcontroller**: PIC16F877A
- **RTC Module**: DS1307 (I2C interface)
- **Display**: 4x Common Anode 7-segment LEDs
- **Driver**: ULN2003 or similar NPN transistor array (for multiplexing)
- **Transistors**: BC557 PNP transistors (4x for digit control)
- **Push Buttons**: 4x (Up, Down, Set, Reset)
- **Resistors**: 
  - 220Ω resistors for LED segments (7x4 = 28 pieces)
  - 10kΩ pull-up resistors for buttons (4x)
  - 10kΩ pull-up resistors for I2C (2x)
- **Capacitors**: 
  - 32.768kHz crystal for DS1307
  - 0.1µF decoupling capacitors
- **Crystal**: 4MHz for PIC16F877A
- **Power Supply**: 5V

## Pin Configuration

### PIC16F877A Pinout
```
PORTA (0-7):        Segment outputs (a, b, c, d, e, f, g, dp)
PORTB (0-3):        Digit selection (via BC557 transistors)
PORTC (3-4):        I2C communication (SCL, SDA) for DS1307
RB4-RB7:            Push button inputs (Up, Down, Set, Reset)
```

### DS1307 I2C Address
- Address: 0x68
- SCL: PORTC.3 (RC3)
- SDA: PORTC.4 (RC4)

### Push Buttons
- RB4: UP button (increment digit/value)
- RB5: DOWN button (decrement digit/value)
- RB6: SET button (cycle through editable fields)
- RB7: RESET button (reset to default)

## Functionality
1. **Display**: Shows time from DS1307 in HH:MM format on 4-digit display
2. **Time Setting**: Use buttons to adjust hours and minutes
3. **Auto-increment**: Long press UP/DOWN for continuous increment/decrement
4. **Reset**: RESET button sets time back to 12:00

## Proteus Simulation
- Import the .dsch file into Proteus Design Suite
- Load the generated .hex file into PIC16F877A
- Run simulation to test button inputs and display output

## File Structure
```
├── firmware/
│   ├── 7segment_display.c
│   ├── i2c_ds1307.c
│   ├── i2c_ds1307.h
│   ├── main.c
│   └── ds1307_registers.h
├── circuit_design/
│   ├── 7segment_circuit.dsch (Proteus schematic)
│   └── README.md
└── documentation/
    └── pin_configuration.txt
```

## Building the Project
1. Open the CCS C Compiler IDE
2. Create a new project for PIC16F877A
3. Add all .c and .h files from the firmware folder
4. Configure project settings:
   - Processor: PIC16F877A
   - Oscillator: HS 4MHz
5. Compile and generate .hex file
6. Load .hex into Proteus simulation

## Schematic Details
The circuit uses:
- **Common Anode Configuration**: Digits powered via BC557 transistors (active LOW)
- **Segment Control**: PORTA outputs directly to LED segments via 220Ω resistors
- **Multiplexing**: Rapid digit switching to display all 4 digits
- **I2C Communication**: DS1307 communicates time data via I2C protocol

---
**Author**: tuandzdz134-hash  
**Date**: 2026-05-08
