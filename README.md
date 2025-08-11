# IoT Electrical Power Monitoring System

Final Project from the class "Electrical Instrumentation and Measurements"

Final project grade of 17 out of 20

This project implements a real-time electrical power monitoring system using an ESP32 microcontroller. 

The system measures voltage, current, power, frequency, and power factor from electrical circuits and transmits the data to ThingSpeak cloud platform for remote monitoring and analysis.

## Features

- **Real-time Electrical Measurements**: 
  - RMS Voltage and Current
  - Active Power and Apparent Power
  - Power Factor
  - Frequency (Voltage and Current)
- **IoT Connectivity**: WiFi connection for data transmission
- **Cloud Integration**: Automatic data upload to ThingSpeak platform
- **High-precision Sampling**: 1000 samples per measurement cycle
- **Automatic Calibration**: Offset compensation for accurate readings

## Hardware Requirements

- **ESP32 Development Board** (or compatible microcontroller)
- **Voltage Sensor/Transducer** (connected to GPIO 34)
- **Current Sensor/Transducer** (connected to GPIO 35)
- **Signal Conditioning Circuit** (for voltage and current measurements)
- **Power Supply** for the ESP32

## Software Requirements

- **Arduino IDE** (version 1.8.x or later)
- **ESP32 Board Support Package**
- **Required Libraries**:
  - `WiFi.h` (included with ESP32)
  - `ThingSpeak.h` (install via Library Manager)

## Installation and Setup

### 1. Hardware Setup

1. Connect the voltage sensor to GPIO pin 34
2. Connect the current sensor to GPIO pin 35
3. Ensure proper signal conditioning for both sensors
4. Connect the ESP32 to your computer via USB

### 2. Software Setup

1. **Install Arduino IDE** and ESP32 board support
2. **Install ThingSpeak Library**:
   - Open Arduino IDE
   - Go to `Tools` > `Manage Libraries`
   - Search for "ThingSpeak" and install the library

3. **Configure Network Settings**:
   - Open `secrets.h`
   - Replace `SECRET_SSID` with your WiFi network name
   - Replace `SECRET_PASS` with your WiFi password
   - Update `SECRET_CH_ID` with your ThingSpeak channel ID
   - Update `SECRET_WRITE_APIKEY` with your ThingSpeak write API key

### 3. ThingSpeak Setup

1. Create a ThingSpeak account at [thingspeak.com](https://thingspeak.com)
2. Create a new channel with the following fields:
   - Field 1: Voltage RMS
   - Field 2: Voltage Frequency
   - Field 3: Current RMS
   - Field 4: Current Frequency
   - Field 5: Active Power
   - Field 6: Apparent Power
   - Field 7: Power Factor

3. Note down your Channel ID and Write API Key

## Usage

### Uploading the Code

1. Open `IME_trabfinal.ino` in Arduino IDE
2. Select the correct board (ESP32) and port
3. Verify and upload the code

### Monitoring Data

1. **Serial Monitor**: Open Serial Monitor (115200 baud) to view real-time measurements
2. **ThingSpeak Dashboard**: Access your ThingSpeak channel to view historical data and charts
3. **Data Transmission**: Data is automatically sent to ThingSpeak every measurement cycle

## Technical Details

### Measurement Process

1. **Sampling**: Collects 1000 samples over a 60-second period
2. **Calibration**: Removes DC offset from voltage and current measurements
3. **Signal Processing**: 
   - Calculates RMS values using root mean square method
   - Determines frequency by measuring zero-crossing periods
   - Computes power factor and apparent power
4. **Data Transmission**: Sends averaged measurements to ThingSpeak

### Key Parameters

- **Sampling Rate**: 1000 samples per cycle
- **Measurement Interval**: 60 seconds
- **ADC Resolution**: 12-bit (0-4095)
- **Voltage Scaling**: 132x (transducer constant)
- **Current Scaling**: 20x (transducer constant)

## File Structure

```
IME/
├── IME_trabfinal.ino    # Main Arduino sketch
├── secrets.h            # Configuration file (WiFi, ThingSpeak credentials)
├── README.md           # This file
└── Relatório final IME_62446_62501_62907_63368.pdf  # Project report
```

## Configuration

### secrets.h Configuration

```cpp
#define SECRET_SSID "YourWiFiNetwork"        // WiFi network name
#define SECRET_PASS "YourWiFiPassword"       // WiFi password
#define SECRET_CH_ID 0000000                 // ThingSpeak channel ID
#define SECRET_WRITE_APIKEY "YourAPIKey"     // ThingSpeak write API key
```


## Contributors

- Iúri Moças
- [Duarte Pereira](https://www.linkedin.com/in/duarte-pereira-349b4b319/)
- [Diogo Ferreira](https://www.linkedin.com/in/diogo-ferreira-9b6210331/)
- [Tiago Monteiro](https://www.linkedin.com/in/tiago-monteiro-/)


