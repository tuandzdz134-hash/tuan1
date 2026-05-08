# Build Instructions for 7-Segment Display Project

## Prerequisites
- **CCS C Compiler** (PICC) for PIC16F877A
- **Proteus Design Suite** (for simulation)
- **MPLAB X IDE** (optional, for additional debugging)

## Compilation Steps

### Option 1: Using CCS C Compiler IDE

1. **Create New Project**
   - Open CCS C Compiler
   - File → New Project
   - Select "Standalone Project"
   - Processor: PIC16F877A

2. **Add Source Files**
   - Right-click on project → Add Files to Project
   - Add these files from the `firmware/` folder:
     - main.c
     - 7segment_display.c
     - i2c_ds1307.c
   - Add header files:
     - 7segment_display.h
     - i2c_ds1307.h
     - ds1307_registers.h

3. **Configure Project Settings**
   - Right-click project → Properties
   - Processor: PIC16F877A
   - Device Clock: 4.000 MHz
   - Optimization: None (for debugging)
   - Output: HEX (Intel)

4. **Build Project**
   - Build → Compile
   - Check for errors in the Build Output window
   - If successful, a .hex file will be generated

### Option 2: Command Line Compilation

```bash
# Navigate to the firmware directory
cd firmware/

# Compile using CCS compiler
CCSC +DF +LN -p16F877A main.c 7segment_display.c i2c_ds1307.c

# Output will be main.hex
```

## Proteus Simulation

### Step 1: Create Circuit in Proteus

1. Open Proteus Design Suite
2. Create new schematic project
3. Follow the schematic guide in `circuit_design/PROTEUS_CIRCUIT.txt`
4. Place and connect all components

### Step 2: Configure Virtual Instruments

1. **Virtual Serial Monitor**
   - Add VSM COM component for debugging output (optional)
   - Connect to UART pins if debugging is needed

2. **Virtual Push Buttons**
   - Configure 4 buttons linked to RB4-RB7
   - Set proper stimulus to simulate button presses

3. **Virtual Power Supply**
   - Set 5V supply
   - Set simulation to continuous run

### Step 3: Load HEX File

1. Double-click the PIC16F877A in the schematic
2. Go to "Program File"
3. Browse and select the compiled .hex file
4. Click OK

### Step 4: Run Simulation

1. Click Play (▶) button or press F12
2. Observe LED display output
3. Click virtual buttons to test functionality
4. Use Debug → Step Over for instruction-level debugging

## Expected Behavior

After successful compilation and loading:

1. **Initial State**
   - Display shows time read from DS1307
   - Default: 12:00 on startup

2. **Button Functionality**
   - **UP (RB4)**: Increment hours (in hours edit mode) or minutes (in minutes edit mode)
   - **DOWN (RB5)**: Decrement hours or minutes
   - **SET (RB6)**: Cycle through modes (Display → Edit Hours → Edit Minutes → Display)
   - **RESET (RB7)**: Reset time to 12:00

3. **Display Indicators**
   - Decimal point shows between hours and minutes in display mode
   - Decimal point blinks on edited field in edit modes

## Troubleshooting

### Compilation Errors

1. **File not found errors**
   - Verify all header files are in the same directory
   - Check include paths in project settings

2. **Processor not recognized**
   - Verify PIC16F877A is selected in device settings
   - Update CCS compiler if using very old version

3. **Undefined reference errors**
   - Ensure all .c files are added to the project
   - Check that function declarations match implementations

### Simulation Issues

1. **LED doesn't light up**
   - Verify PORTA is configured as outputs
   - Check 220Ω resistor values
   - Verify common anode connection

2. **Buttons don't respond**
   - Configure button stimulus correctly in Proteus
   - Verify pull-down resistor connections
   - Check RB4-RB7 are set as inputs (TRISB)

3. **I2C communication fails**
   - Verify RC3 and RC4 are configured as inputs
   - Check 10kΩ pull-up resistors on SCL/SDA
   - Ensure DS1307 is addressed at 0x68

4. **Time not updating**
   - Check DS1307 crystal oscillation
   - Verify I2C communication in simulation
   - Use timing display or breakpoints to debug I2C

## Advanced Options

### Enable Debugging Output

Uncomment debug statements in main.c to output status via UART:
```c
// #define DEBUG_ENABLED
```

### Adjust Timing

- Modify `delay_ms()` function for different oscillator frequencies
- Change Timer0 reload value for different interrupt rates
- Adjust debounce counters for button sensitivity

### Change Initial Time

In main.c, modify the global variable:
```c
DS1307_Time current_time = {0, 0, 12, 1, 1, 1, 26};
                          //sec, min, hr, day, date, month, year
```

## Hardware Programming

When ready to program actual hardware:

1. **Using PIC Programmer**
   - Use MPLAB X IDE with appropriate programmer (PICKit 3, etc.)
   - Load .hex file from CCS compiler output
   - Select correct device: PIC16F877A
   - Program and verify

2. **Using CCS Built-in Programmer**
   - Plug in programmer device
   - Open .hex file in CCS
   - Click Program button
   - Wait for completion

## Files Generated

After successful build:
```
firmware/
├── main.hex          (Loadable HEX file)
├── main.lst          (Assembly listing)
├── main.o            (Object file)
└── main.cof          (Debug symbol file)
```

Use `main.hex` for Proteus simulation and hardware programming.

---

For detailed circuit connections, see: `circuit_design/PROTEUS_CIRCUIT.txt`
For complete project overview, see: `circuit_design/README.md`
