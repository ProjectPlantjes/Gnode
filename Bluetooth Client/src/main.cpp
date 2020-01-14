#include <BLE_Client.h>
#include <string>


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

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// This is the Arduino setup loop function.
void setup() {
  
  Serial.begin(115200);
  ++bootCount;

  initTemperature(); // Initialize the temperature sensor
  initHumidity(); // Initialize the humidity sensor
  initRainSensor(); // Initialsize the rainsensor
  initGPS(); // Initialize the GPS module
  BLE_Init(); // Initialize the BLE

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  //" Seconds");
}

// This is the Arduino main loop function.
void loop() {
  connectToBLEServer();

  if (BLEConnection()) {

  //std::ostringstream count;
  //count << std::fixed << bootCount;

  // Init variable oss and assign the JSON stream to this variable. 
  std::ostringstream oss;
  oss << "{\"temperature\":\"" << requestTemperature() << "\",\"Rain\":\"" << requestRainSensor() << "\",\"Humidity\":\"" << requestHumidity() << "\",\"Lat\":\"" << requestGPSLat() << "\",\"Lng\":\"" << requestGPSLng() << /*"\",\"BootCount\":\"" << bootCount <<*/ "\"}";
  BLE_SendMessasage(oss.str()); // Send the message stores in oss variable

  delay(100); // Make this delay(100) if want to receive message as well
  
  // Start DeepSleep of the ESP32
  esp_deep_sleep_start();
  }
} // End of loop