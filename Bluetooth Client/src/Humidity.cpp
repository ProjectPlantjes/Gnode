// Standard Libraries
#include <Arduino.h>
#include <string>
#include <sstream>
#include <iomanip>

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
std::string requestHumidity()
{
  humidity = am2320.readHumidity(); //Request and save the humidity value

  std::ostringstream strHumidity;
  strHumidity << std::fixed << std::setprecision(2) << humidity;

  //std::string strHumidity = std::string(humidity, 2); // Convert the humidity value to a String

  return strHumidity.str(); // Return the humidity value as a String
}
