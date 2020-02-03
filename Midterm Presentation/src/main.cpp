#include <Arduino.h>
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

// Store the recieved characters (up to three) and store them into stringvalue
String stringValue1;
String stringValue2;
String stringValue3;
String stringValue;

// Contains the longitude and latitude
float lng;
float lat;

std::string actualMessage; // Contains the final message, in Json format, that will be send over Bluetooth
std::string rainDetected; // Contains wether there is rain detected or not

// constants won't change. Used here to set a pin number :
const long ledBlinkInterval = 5000;	// interval at which to blink (milliseconds) (Delay)

// Variables will change :
float temp_0; // Contains the read temperature value
int once = 0; // Makes sure that certain messages only display once on Bluetooth connection
unsigned long previousMillis = 0;	// will store last time LED was updated

// Bluetooth stuff
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 100;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ONE_WIRE_BUS 2 // DS18B20 on NodeMCU pin D4  
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature DS18B20(&oneWire); 

#define RXPin 12
#define TXPin 13
#define GPSBaud 9600
TinyGPSPlus gps;
HardwareSerial ss(1);

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

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*");
        Serial.print("New value: ");
        stringValue.clear();
        for (int i = 0; i < value.length(); i++) {
          Serial.print(value[i]);

          if (i+5 == value.length()) {
            stringValue1 = value[i];
            stringValue += stringValue1;
          }

          if (i+4 == value.length()) {
            stringValue2 = value[i];
            stringValue += stringValue2;
          }

          if (i+3 == value.length()) {
            stringValue3 = value[i];
            stringValue += stringValue3;
            Serial.println();
            Serial.print("stringValue: ");
            Serial.println(stringValue);
          }
        }
        
        Serial.println();
        Serial.println("*");
      }
    }
};

void setup() {
  Serial.begin(115200);
  DS18B20.begin(); 
  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  pinMode(17, INPUT);

  // Create the BLE Device
  BLEDevice::init("Touch my ding ding dong");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  // Update the currentMillis time for no delay function
  unsigned long currentMillis = millis();
  
  // Enter If statement when external Bluetooth device is connected
  if (deviceConnected) {


    // Do this If statement only the first time a Bluetooth device is connected
    if (once == 0){
      Serial.println("External device connected through Bluetooth");
      once = 1;
    }
    
    // Enter IF statement when character "1" is recieved from externel device
    if (stringValue == "1") {

      while (ss.available() > 0) {
        gps.encode(ss.read());
      }
      if (runEvery_gps(5000)) 
      {
      lat = gps.location.lat();
      lng = gps.location.lng();
      }

      // Convert int temp_0 to std::string temperature
      std::string strLat;
      std::stringstream ssLat;
      ssLat << std::fixed << std::setprecision(6) << lat;
      strLat = ssLat.str();

      // Convert int temp_0 to std::string temperature
      std::string strLng;
      std::stringstream ssLng;
      ssLng << std::fixed << std::setprecision(6) << lng;
      strLng = ssLng.str();

      // Request the temperature
      DS18B20.requestTemperatures();  
      temp_0 = DS18B20.getTempCByIndex(0); // Sensor 0 will capture Temp in Celcius

      // Convert float temp_0 to std::string temperature
      std::string temperature;
      std::stringstream ssTemp;
      ssTemp << temp_0;
      temperature = ssTemp.str();

      // Delay with no Delay function
      if (currentMillis - previousMillis >= ledBlinkInterval) {
		    previousMillis = currentMillis;
        
          //Read digital output data from the rain sensor
          if(digitalRead(17) == HIGH) {
            Serial.println("No Rain Detected");
            rainDetected = "No Rain Detected";
          }
          else {
            Serial.println("Rain Detected");
            rainDetected = "Rain Detected";
          }

        // Combine all recieved data (std::string type) into one variable (Json format)
        actualMessage = "{\"temperature\":\"" + temperature + "\",\"Rain\":\"" + rainDetected + "\",\"Lat\":\"" + strLat + "\",\"Lng\":\"" + strLng + "\"}";

        // Send the complete string, in Json format, to the external device
        pCharacteristic->setValue(actualMessage);
        pCharacteristic->notify(); // notify changed value
        delay(3); // Make sure the buffer doesn't overload by adding delay(3)
        pCharacteristic->setValue("\n"); 
        pCharacteristic->notify(); // notify changed value
      }
    }
 }
     
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
 
    once = 0;
    stringValue.clear();

    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
  }
}