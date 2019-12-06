// Standard Libraries
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <sstream>
#include <iomanip>

// Own Libraries
#include "Temperature.h"
#include "GPS.h"
#include "Humidity.h"
#include "RainSensor.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth stuff
BluetoothSerial SerialBT;
String actualMessage;

void setup() {
  
  // Open serial port and wait until the port is available
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // hang out until serial port opens
  }

  initTemperature(); // Initialize the temperature sensor
  initHumidity(); // Initialize the humidity sensor
  initRainSensor(); // Initialsize the rainsensor
  initGPS(); // Initialize the GPS module

  SerialBT.begin("Right in the Putty!"); //Bluetooth device name
  //Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  // Combine all data into one Json stream and send the data over Bluetooth
  actualMessage = "{\"temperature\":\"" + requestTemperature() + "\",\"Rain\":\"" + requestRainSensor() + "\",\"Humidity\":\"" + requestHumidity() + "\",\"Lat\":\"" + requestGPSLat() + "\",\"Lng\":\"" + requestGPSLng() + "\"}";
  SerialBT.println(actualMessage);
  Serial.println(actualMessage);

  // Flush the data so the buffer doesn't overload  
  SerialBT.flush();
  delay(1000);
}
