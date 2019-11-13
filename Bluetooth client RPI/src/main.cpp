#include <Arduino.h>
#include "BluetoothSerial.h"
#include <string>
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <sstream>
#include <iomanip>
#include <TinyGPS++.h>
#include <HardwareSerial.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Defines for the temperature sensor
#define ONE_WIRE_BUS 2 // DS18B20 on NodeMCU pin D4  
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature DS18B20(&oneWire); 

// Defines for the GPS module
#define RXPin 12
#define TXPin 13
#define GPSBaud 9600
TinyGPSPlus gps;
HardwareSerial ss(1);

// Bluetooth stuff
BluetoothSerial SerialBT;
String actualMessage;
float temp_0; // Contains the read temperature value
String rainDetected;
float humidity;

// Contains the longitude and latitude
float lng;
float lat;

// Adafruit sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();

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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // hang out until serial port opens
  }

  DS18B20.begin(); // Init temperature sensor
  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // Init GPS module
  am2320.begin(); // Init humidity signal
  pinMode(17, INPUT); // Configure input pin for rain sensor

  SerialBT.begin("Right in the Putty!"); //Bluetooth device name

  Serial.println("Adafruit AM2320 Basic Test");
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  // Request GPS data
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
  if (runEvery_gps(5000)) 
  {
    lat = gps.location.lat();
    lng = gps.location.lng();
  }
      
  // Request the temperature
  DS18B20.requestTemperatures();  
  temp_0 = DS18B20.getTempCByIndex(0); // Sensor 0 will capture Temp in Celcius

  //Request the humidity
  humidity = am2320.readHumidity();

  //Read digital output data from the rain sensor
  if(digitalRead(17) == HIGH) {
    Serial.println("No Rain Detected");
    rainDetected = "No Rain Detected";
  }
  else {
    Serial.println("Rain Detected");
    rainDetected = "Rain Detected";
  }
      
  // Convert Float to String (GPS data)
  String strLat = String(lat, 6);
  String strLng = String(lng, 6);

  // Convert Float to String (temperature)
  String temperature = String(temp_0, 2);

  // Convert Float to String (humidity)
  String strHumidity = String(humidity, 2);

  // Combine all data into one Json stream and send the data over Bluetooth
  actualMessage = "{\"temperature\":\"" + temperature + "\",\"Rain\":\"" + rainDetected + "\",\"Humidity\":\"" + strHumidity + "\",\"Lat\":\"" + strLat + "\",\"Lng\":\"" + strLng + "\"}";
  SerialBT.println(actualMessage);

  // Flush the data so the buffer doesn't overload  
  SerialBT.flush();
  delay(1000);
}
