# Gnode

## Hardware
For this project we use the Espressif ESP32 processor. During development we have the Lolin-D32 development board

## Sensors
 For the Gnode we want the following sensors:

 - Temperature
 - Humiditry
 - Some gasses
 - Rain detection
 - Air pressure
 - GPS

## Communication
For communication we use the ESP32 onboard BLE

## Problems
If you use the same hardware as we have, you can have the following problems

### While uploading Error(2)
To solve this, connect pin 0 to GND during uploading. This puts the ESP32 in flash-mode
