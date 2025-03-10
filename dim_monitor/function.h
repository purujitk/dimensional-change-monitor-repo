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
#include <HX711.h> //HX711 by bogdan
#include <WiFiUdp.h>

////////////WIFI PARAMATERS//////////////
// #define WIFI_SSD "QueensU-Secure" //change depending on wifi client
// #define WIFI_USER "23mt27"
// #define WIFI_PASS "qj5sTXn5E_-mjs9"

#define WIFI_SSD "Purujit" //change depending on wifi client
#define WIFI_PASS "PurujitK"

// #define WIFI_SSD "Ludacris-5G" //change depending on wifi client
// #define WIFI_PASS "#Stanley2025!"

extern WiFiClient client;
extern WiFiUDP ntpUDP;

// ////////////THINGSPEAK DATA/////////////
extern const char* W_API_KEY;
extern const char* R_API_KEY;
extern unsigned long CHANNEL_ID;
// #define W_API_KEY "LG11PSUFXN66T3DA"
// #define R_API_KEY "EMTZQ7KAPR04KHKB"
// #define CHANNEL_ID "2865189"

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

#define INTERVAL 900


///////////HX711 PIN ASSIGNMENTS//////////////

#define CLOCK_LINE 4
#define DATA_LINE 16

extern HX711 scale;

#define CALIBRATION_FACTOR

////////////DIMENSIONAL DATA////////////////

//THIS DATA WAS RECORDED UNDER STAP

const float INITIAL_LENGTH_LONG = 0; //or x,y,z not sure
const float INITIAL_LENGTH_RAD = 0;
const float INITIAL_LENGTH_TANG = 0;


///////////function initialization////////////

void wifiinit(); //wifi initialization 
void thingspeaktransmit(int sensordata);
void logToSD(float strain); // Updated to include status
void sdInit();
void HX711_init();
float get_deformation(int dim);


#endif

