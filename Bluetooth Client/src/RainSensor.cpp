// Standard Libraries
#include <Arduino.h>

// Own Libraries
#include "RainSensor.h"

// Contains the status of the rain sensor
std::string rainDetected;

/* 
// Initialize the rain sensor 
*/ 
void initRainSensor(){
  pinMode(17, INPUT); // Configure input pin for rain sensor
}

/* 
// Request if there's rain detected, return the requested value as a String 
*/
std::string requestRainSensor(){

  //Read digital output data from the rain sensor
  if(digitalRead(17) == HIGH) {
    Serial.println("No Rain Detected"); 
    rainDetected = "No Rain Detected"; // Save that there's no rain detected
  }
  else {
    Serial.println("Rain Detected");
    rainDetected = "Rain Detected"; // Save that there's rain detected
  }
  return rainDetected; // Return the status of the rain sensor as a String
}