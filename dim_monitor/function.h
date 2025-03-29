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
#include <HX711.h> //HX711 by bogdan necula
#include <WiFiUdp.h> //for time stamping
#include <ESP_Mail_Client.h>
#include <time.h>

////////////WIFI PARAMATERS//////////////
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

// #define CHANNEL_ID "2865189"
// extern const char* W_API_KEY = "LG11PSUFXN66T3DA";
// extern const char* R_API_KEY = "EMTZQ7KAPR04KHKB";
// extern unsigned long CHANNEL_ID = 865189;

#define SERVER "api.thingspeak.com"

//THINGSPEAK ERROR CODES
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

#define INTERVAL 15000


///////////HX711 PIN ASSIGNMENTS//////////////
#define CLOCK_LINE 4
#define DATA_LINE 16

extern HX711 scale;

#define CALIBRATION_FACTOR 1

////////////DIMENSIONAL DATA////////////////

//THIS DATA WAS RECORDED UNDER STAP hopefully

const float INITIAL_LENGTH = 1; //or x,y,z not sure
// const float INITIAL_LENGTH_RAD = 0;
// const float INITIAL_LENGTH_TANG = 0;

///////////STRAIN GUAGE DATA//////////////////
#define GUAGE_FACTOR 2.0
#define EXC_VOLT 2.24
extern long baseline;

extern float rawToVoltage_theoretical;

////////////////////EMAIL/////////////////////////////
/** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "strainguage839@gmail.com"
#define AUTHOR_PASSWORD "vpqthieieiwmsoqn"

/* Recipient's email*/
// #define RECIPIENT_EMAIL "rogers.hannahrose@gmail.com"
#define RECIPIENT_EMAIL "purujitkantiya@gmail.com"


extern SMTPSession smtp;
extern Session_Config config;


///////////function initialization////////////

void wifiinit(); //wifi initialization 
int thingspeaktransmit(int sensordata, int field);
void logToSD(float data, String severity, String name); // Updated to include status
String create_file(String formatteddate);
void sdInit();
void HX711_init();
float get_strain(float baseline);
float get_nominal_reading();
void email_init();
void send_email(String name);
void smtpCallback(SMTP_Status status);
String formatted_local_time();
String getFormattedDate();
String check_severity(float data, float baseline);


#endif

