// Standard Libraries
#include <Arduino.h>

// Installed Libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

// Own Libraries
#include "Humidity.h"

// Adafruit sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();
float humidity;

/*
// Initialize the humidity sensor
*/
void initHumidity()
{
  am2320.begin();
}

/* 
// Request the humidity, return requested value as a String
*/
String requestHumidity()
{
  humidity = am2320.readHumidity(); //Request and save the humidity value
  Serial.println(humidity);
  String strHumidity = String(humidity, 2); // Convert the humidity value to a String

  return strHumidity; // Return the humidity value as a String
}
