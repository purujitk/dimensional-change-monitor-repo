/*
dim_monitor.ino

by:Purujit Kantiya, Abedal rehman shehabi, Hannah Rogers, Valentine Adeseyie, Gabriella Belanger

Dimensional change monitor main script for APSC 103 project

client: Ingenium
*/

//THINGSPEAK TRANSIMISSION RATE IS MINIMUM 15 SECONDS!!!!
//HAVE TO WAIT FIFTEEN SECONDS BEFORE TRANSMITTING FOR A SECONDS TIME


#include "function.h"


WiFiClient client;
WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); MIGHT NOT BE NEEDED CAN JUST USE CONFIG TIME
HX711 scale;
SMTPSession smtp;
Session_Config config;

const char* W_API_KEY = "LG11PSUFXN66T3DA";
const char* R_API_KEY = "EMTZQ7KAPR04KHKB";
unsigned long CHANNEL_ID = 865189;

unsigned long prev_time = 0;
long baseline = 0;

unsigned long time_dif = 0;

String date = "";
String name = "";


void setup() {
  
  //initializations

  //initialize sd card
  sdInit();

  //initialize email
  email_init();

  //initialize the hx711
  HX711_init();

  //initialize wifi
  wifiinit();

  //initialize serial monitor
  Serial.begin(115200);

  //getting baseline value
  baseline = get_baseline();

  //getting inital date
  date = getFormattedDate();

  //creating initial file
  name = create_file(date);


}

void loop() {

  timeClient.upate();

  time_dif = (timeClient.getEpochTime() - prev_time);

  if(Wifi.status() != WL_CONNECTED){
    for(int i = 0; i < 3; i++){
      WiFi.reconnect();
    }
  }

  if (time_dif >= 900){

    data = get_deformation(baseline);

    if(getFormattedDate() != date){

      if(WiFi.status() == WL_CONNECTED){
        send_email(name);
      }else
        //we can store the date that could not be sent in an array possibly and then check to send that data later

      date = getFormattedDate();
      name = create_file(date);

    }else 
      logToSD(data, check_severity(data, baseline), name);


  }else
    //sleep for some time


}

