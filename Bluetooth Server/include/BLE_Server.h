#include <Arduino.h>


void BLE_Init(std::string BLE_Name); 
bool BLE_Connection();
void BLE_SendMessage(std::string Message);
String BLE_ReceiveMessage();
