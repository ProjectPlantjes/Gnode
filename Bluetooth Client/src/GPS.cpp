// Standard Libraries
#include <Arduino.h>
#include <string>
#include <sstream>
#include <iomanip>

// Installed Libraries
#include <TinyGPS++.h>
#include <HardwareSerial.h> 

// Own Libraries
#include "GPS.h"

// Defines for the GPS module
#define RXPin 12
#define TXPin 13
#define GPSBaud 9600
TinyGPSPlus gps;
HardwareSerial ss(1);

// Contains the longitude and latitude value
float lng;
float lat;

boolean gps_read(){
  return (gps.location.isValid());
}

float gps_latitude(){
  return gps.location.lat();
}
float gps_longitude(){
  return gps.location.lng();
}

float gps_meters() {
  return gps.altitude.meters();
}

float gps_HDOP(){
  if (gps.hdop.isValid())
    return gps.hdop.hdop();
  else
    return 40.0;
}

String gps_location()
{
  if (gps.location.isValid())
  {
    String str = "";
    str += gps_latitude();
    str += " ";
    str += gps_longitude();
    return str;
  }
  else
  {
    return "#### ####";
  }  
}

String gps_date()
{
  if (gps.date.isValid())
  {
    int d = gps.date.day();
    int m = gps.date.month();
    int y = gps.date.year();
    String str = "";
    if (d < 10) str += "0";
    str += d;
    str += "/";
    if (m < 10) str += "0";
    str += m;
    str += "/";
    str += y;
    return str;
  }
  else
  {
    return "xx/xx/xxxx";
  }  
}

String gps_time()
{
  if (gps.time.isValid())
  {
    int s = gps.time.second();
    int m = gps.time.minute();
    int h = gps.time.hour();
    String str = "";
    if (h < 10) str += "0";
    str += h;
    str += ":";
    if (m < 10) str += "0";
    str += m;
    str += ":";
    if (s < 10) str += "0";
    str += s;
    return str;
  }
  else
  {
    return "xx:xx:xx";
  }  
}

boolean runEvery_gps(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.println();
}

// Initialize the GPS module
void initGPS(){
  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); 
}

/* 
// Request the Latitude value, return the requested value as a String
*/
std::string requestGPSLat(){

  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
    lat = gps.location.lat(); // Request and save the latitude value

    std::ostringstream strLat;
    strLat << std::fixed << std::setprecision(6) << lat;

    return strLat.str(); // Return the latitude value as a String
}

/* 
// Request the Longitude value, return the requested value as a String
*/
std::string requestGPSLng(){
    
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
    lng = gps.location.lng(); // Request and save the latitude value

    std::ostringstream strLng;
    strLng << std::fixed << std::setprecision(6) << lng;

    return strLng.str(); // Return the latitude value as a String
}

std::string requestTime(){
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

    std::ostringstream stream;
    int tsHour = gps.time.hour();
    int tsMinute = gps.time.minute();
    int tsSecond = gps.time.second();
    stream << tsHour << ":" << tsMinute << ":" << tsSecond;
    std::string str = stream.str();

    return stream.str(); // Return the latitude value as a String
}