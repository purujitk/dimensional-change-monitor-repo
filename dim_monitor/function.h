/*
Function.h

by: Purujit Kantiya
*/

/////////INCLUDING LIBRARIES///////////
#include <WiFi.h>
#include <ThingSpeak.h>

////////////WIFI PARAMATERS//////////////
const char* ssid = "Ludacris-5G";
const char* password = "#Stanley2025!";


////////////THINGSPEAK DATA/////////////
const char* w_api_key = "LG11PSUFXN66T3DA";
const char* r_api_key = "EMTZQ7KAPR04KHKB";
const int channel_id = 2865189;
const char* server = "api.thingspeak.com";


///////////function initialization////////////

void wifiinit(const char* ssid, const char* password); //wifi initialization 

