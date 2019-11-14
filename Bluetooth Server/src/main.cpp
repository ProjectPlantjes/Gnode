#include <BLE_Server.h>

void setup() {
  Serial.begin(115200);
  BLE_Init("BramsServer");
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
    if(BLE_Connection())
    {
       BLE_SendMessage("Bram is leip\n");
    }
}