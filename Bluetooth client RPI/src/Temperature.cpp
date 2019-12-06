// Standard Libraries
#include <Arduino.h>

// Installed Libraries
#include <DallasTemperature.h> 
#include <OneWire.h> 

// Own Libraries
#include "Temperature.h"

// Contains the temperature value
float temp_0; 

// Defines for the temperature sensor
#define ONE_WIRE_BUS 2 // DS18B20 on NodeMCU pin 2  
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature DS18B20(&oneWire); 

/* 
// Initialize the temperature sensor 
*/
void initTemperature(){
  DS18B20.begin(); // Init temperature sensor
}

/* 
// Request the temperature, return the requested value as a String 
*/
String requestTemperature(){

  DS18B20.requestTemperatures();  
  temp_0 = DS18B20.getTempCByIndex(0); //Request and save the humidity value in Celsius

  String temperature = String(temp_0, 2); // Convert the temperature value to a String

  return temperature; // Return the temperature value as a String

}