# Smart Home ESP32 Project

This project runs on an ESP32 NodeMCU-32S board and monitors temperature, humidity, motion, and light level.
It uses a DHT22 sensor, an LDR light sensor, a potentiometer for temperature setpoint adjustment, and a PIR motion sensor.

## Features

- Reads temperature and humidity from a DHT22 sensor
- Reads light level from an LDR sensor
- Uses a potentiometer to set the temperature threshold
- Detects motion with a PIR sensor
- Controls 3 LEDs:
  - Red LED for thermostat status
  - Green LED for PIR motion alarm
  - Yellow LED for darkness indicator
- Prints status information to the serial monitor

## Pin Mapping

| Sensor / LED | ESP32 Pin |
|--------------|-----------|
| DHT22 data   | GPIO4     |
| LDR input    | GPIO34    |
| Potentiometer| GPIO35    |
| PIR motion   | GPIO13    |
| Red LED      | GPIO25    |
| Green LED    | GPIO26    |
| Yellow LED   | GPIO27    |

## Build and Upload

1. Open the project in PlatformIO.
2. Build the project.
3. Upload the firmware to the ESP32.
4. Open the serial monitor at `115200` baud.

## Notes

- The DHT22 sensor is read every 2.5 seconds.
- The serial status is updated every 1 second.
- Motion alarms remain active for 5 seconds after the last detected motion.
- The potentiometer sets the temperature threshold between 15°C and 40°C.
