// Standard Libraries
#include <Arduino.h>
#include "BluetoothSerial.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE_Client.h>
#include <sstream>
#include <iomanip>
#include <string>
#include "esp_system.h"

// Own Libraries
#include "Temperature.h"
#include "GPS.h"
#include "Humidity.h"
#include "RainSensor.h"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */

// Stores the amount of time that the ESP has rebooted from DeepSleep
RTC_DATA_ATTR int bootCount = 0;

const int wdtTimeout = 10000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

// Callback function when the watchdog timer is expired
void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

// This is the Arduino setup loop function.
void setup() {
  
  Serial.begin(115200); // Open serial connection
  ++bootCount; // Increase the bootcount so the program knows how many times the ESP32 has woken up by DeepSleep
  
  initTemperature(); // Initialize the temperature sensor
  initHumidity(); // Initialize the humidity sensor
  initRainSensor(); // Initialsize the rainsensor
  initGPS(); // Initialize the GPS module
  BLE_Init(); // Initialize the BLE

  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  //" Seconds");

  // Reset the Watchdog timer everytime the ESP32 reboots
  timerWrite(timer, 0); //reset timer (feed watchdog)

}

// This is the Arduino main loop function.
void loop() {

  // Connect to the BLE server
  connectToBLEServer();

  // BLE connection is established
  if (BLEConnection()) {
    
    // Init variable count and assign the bootcount to this variable.
    std::ostringstream count;
    count << std::fixed << bootCount;

    // Init variable oss and assign the JSON stream to this variable. 
    std::ostringstream oss;
    oss << "{\"temperature\":\"" << requestTemperature() << "\",\"Rain\":\"" << requestRainSensor() << "\",\"Humidity\":\"" << requestHumidity() << "\",\"Lat\":\"" << requestGPSLat() << "\",\"Lng\":\"" << requestGPSLng() << "\",\"Time\":\"" << requestTime() << "\",\"BootCount\":\"" << bootCount << "\"}";
    BLE_SendMessasage(oss.str()); // Send the message stores in oss variable
    //BLE_SendMessasage("testicles 69"); 

    delay(50); // Make this delay(100) if want to receive message as well
  
    // Start DeepSleep of the ESP32
    esp_deep_sleep_start();
  }
} // End of loop