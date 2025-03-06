/*
function.h

by: Purujit Kantiya
*/

#ifndef FUNCTION_H
#define FUNCTION_H

/////////INCLUDING LIBRARIES///////////
#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <SD.h>
#include <NTPClient.h> //for date time stamps

////////////WIFI PARAMATERS//////////////
#define WIFI_SSD "Ludacris-5G"
#define WIFI_PASS "#Stanley2025!"

extern wifiClient client;

// ////////////THINGSPEAK DATA/////////////
#define W_API_KEY "LG11PSUFXN66T3DA"
#define R_API_KEY "EMTZQ7KAPR04KHKB"
#define CHANNEL_ID "2865189"
#define SERVER "api.thingspeak.com"

#define API_FAIL 404
#define VAL_RANGE -101
#define INV_FIELD -201
#define SET_FIELD -210
#define CONEC_FAIL -301

///////////SD CARD MOD////////////////
#define SD_CS_PIN 5

///////////TIME STAMPING///////////////
#define NTP_OFFSET 0    // Offset in seconds (e.g., 0 for UTC, 3600 for UTC+1)
#define NTP_INTERVAL 60 * 1000 // Update every 60 seconds
#define NTP_SERVER "pool.ntp.org"
extern NTPClient timeClient;

#define INTERVAL 15*60;

///////////function initialization////////////

void wifiinit(); //wifi initialization 
void thingspeaktransmit(int sensordata);
void logToSD(float strain); // Updated to include status
void sdInit();
void updatetime();

#endif

