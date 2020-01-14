#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE_Server.h>

String BLEMessage;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        BLEMessage.clear();

        for (int i = 0; value.length() > i; i++){
          BLEMessage += value[i];
        }
        Serial.println("******************************");
        Serial.print("BLEMessage: ");
        Serial.println(BLEMessage);
        
        Serial.println("******************************");
        Serial.println();
      }
    }
};


/* FUNCTION TO SET UUIDS MAYBE?? */

/*************************************************************
 * Function：BLE_Init
 * Description：Initialises and creates a BLE Server network
   with a name that has been specified by the user.
 * Input parameter：std::string BLE_Name. Name of BLE Server
 * Return：-
 *************************************************************/
void BLE_Init(std::string BLE_Name)
{
  // Create the BLE Device
  BLEDevice::init(BLE_Name);

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

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}


/*************************************************************
 * Function：BLE_Connection
 * Description：Checks if the client has been connected or
   disconnected. When disconnected it starts advertising again
 * Input parameter：-
 * Return：Checks if a device is connected or not. Return 1 
   when connected and 0 when disconnected
 *************************************************************/
bool BLE_Connection()
{
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      //Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
  }

  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
  }

  //return if connected or not
  if(deviceConnected == 1 ) {
    return 1;
  }
  else
  {
    return 0;
  }
}


/*************************************************************
 * Function：BLE_SendMessage
 * Description：Sends a message to all the clients in the 
   created BLE network in std::string format
 * Input parameter：std::string Message. The to be send message
   for the client
 * Return：-
 
      DELAY IN THIS FUNCTION. FOR TESTING REASONS INCLUDED
 *************************************************************/
void BLE_SendMessage(std::string Message)
{
  pCharacteristic->setValue(Message);
  pCharacteristic->notify();
  delay(1000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
}


/*************************************************************
 * Function：BLE_ReceiveMessage
 * Description: Returns the from the client received message
 * Input parameter：-
 * Return：The received message
 *************************************************************/
String BLE_ReceiveMessage()
{
  return BLEMessage;
}
