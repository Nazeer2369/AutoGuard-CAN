# AutoGuard-CAN

An automotive safety and monitoring system using LPC2129, CAN Bus, and MMA7660 accelerometer for real-time accident detection, fuel monitoring, and vehicle indicator control.

## Overview

AutoGuard CAN is an embedded automotive safety system developed using the LPC2129 ARM7 microcontroller. The system utilizes CAN (Controller Area Network) communication for reliable data exchange between multiple vehicle nodes and employs the MMA7660 3-axis accelerometer for accident detection.

The system continuously monitors vehicle motion, fuel level, and indicator status. During emergency conditions, it detects abnormal acceleration values, transmits crash alerts through the CAN network, activates hazard indicators, and provides airbag deployment indication.

#Project flow
                         START
                           │
                           ▼
                 Initialize LPC2129
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
   LCD Initialization  CAN Initialization  I²C Initialization
                           │
                           ▼
               MMA7660 Accelerometer Initialization
                           │
                           ▼
              EINT0 / EINT2 Interrupt Initialization
                           │
                           ▼
                        Main Loop
                           │
                           ▼
  ├─────────────►Read X, Y, Z Data from MMA7660 Sensor
  |                         │
  |                         ▼
  |                 Crash Detected?
  |                  ┌─────┴─────┐
  |                  │           │
  |                 NO          YES
  |                  │           │
  |                  ▼           ▼
  |        Check Indicator   Display
  |           Requests     "AIRBAG OPENED"
  |                 │           │
  |                  ▼           ▼
  |         Monitor Fuel     Send CAN
  |             Status      Danger Message
  |                  │           │
  |                  ▼           ▼
  |        Receive Fuel     Hazard Lights ON
  |          Data via CAN   (Blink Continuously)
  |                  │           │
  |                  ▼           ▼
  |        Update LCD with   Airbag Alert /
  |       Fuel & Indicator   Emergency Mode
  |              Data             
  |                  │             
  |__________________|
                           
                      
## Features

### Crash Detection
- Real-time monitoring of X, Y, and Z acceleration values
- Accident detection using predefined thresholds
- Emergency CAN message transmission
- Airbag deployment indication
- Hazard light activation

### Fuel Monitoring
- Fuel level measurement using ADC
- Fuel percentage display on LCD
- CAN-based fuel data communication

### Vehicle Indicator Control
- Left indicator control using EINT0
- Right indicator control using EINT2
- LCD arrow indication
- CAN-based indicator control

## Hardware Components

- LPC2129 ARM7 Microcontroller
- MCP2551 CAN Transceiver
- MMA7660 3-Axis Accelerometer
- 20x4 LCD Display
- Fuel Gauge Sensor
- LEDs
- Push Buttons (EINT0 and EINT2)
- USB-to-UART Converter

## Software Tools

- Embedded C
- Keil µVision
- Flash Magic

## Communication Protocols

### CAN Bus
- Crash alert transmission
- Fuel data exchange
- Indicator control messages
- Inter-node communication

### I²C Protocol
- MMA7660 sensor interfacing
- Sensor initialization
- Reading acceleration values

## System Architecture

### Main Node
- Reads accelerometer data
- Detects accident conditions
- Sends emergency CAN messages
- Receives fuel information
- Controls indicator requests

### Fuel Node
- Reads fuel sensor using ADC
- Calculates fuel percentage
- Sends fuel data via CAN

### Indicator Node
- Receives CAN messages
- Controls left and right indicators
- Activates hazard lights during emergencies

## Working Principle

1. Initialize LCD, CAN, I²C, Accelerometer, and External Interrupts.
2. Read acceleration values continuously.
3. Check for crash conditions.
4. If a crash is detected:
   - Display airbag alert.
   - Send emergency CAN message.
   - Activate hazard indicators.
5. If no crash is detected:
   - Monitor fuel level.
   - Process indicator requests.
   - Update LCD display.
6. Repeat continuously.

## Results

- Successfully implemented CAN communication between multiple nodes.
- Achieved real-time crash detection using accelerometer data.
- Implemented fuel monitoring using ADC.
- Developed interrupt-based vehicle indicator control.
- Generated emergency alerts and airbag indications during accident conditions.

## Future Enhancements

- GSM-based emergency notification system
- GPS vehicle tracking
- Mobile application integration
- Cloud-based monitoring
- SD card data logging
- Advanced crash severity analysis


## Keywords

Embedded Systems, LPC2129, CAN Bus, MCP2551, MMA7660, ARM7, Embedded C, Automotive Electronics, Vehicle Safety, Accident Detection, Fuel Monitoring, I²C Communication, Real-Time Systems.
