#include <Arduino.h>

/*
// Initialize the GPS module
*/
void initGPS();

/*
// Request the Latitude value, return the requested value as a String
*/
std::string requestGPSLat();

/*
// Request the Longitude value, return the requested value as a String
*/
std::string requestGPSLng();

std::string requestTime();