
# Hydroponics NFT system sensors

## Components

### Wemos D1 R32 (ESP32)

### DS18B20 Temperature Sensor
* 4.7k pullup resistor on data
* data pin 26 (1-wire)

### TDS Sensor
* data pin 39 (ADC)

### pH-4502C pH sensor
* voltage divider 2k and 1k
* data pin 36 (ADC)

### Water Level Sensor
* car gas/water level floater (a variable resistor 140 to 30 ohms)
* voltage divider with the sensor and 200 ohm resistor
* data pin 34 (ADC)

### 4 relay module
* cut both ground and 5V to the ph and tdd sensor
* they mess with readings if they are connected at the same time
* 2 relays for TDS to pin 4
* 2 relays for PH  to pin 5

