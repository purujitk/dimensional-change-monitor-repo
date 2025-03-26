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
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
HX711 scale;

const char* W_API_KEY = "LG11PSUFXN66T3DA";
const char* R_API_KEY = "EMTZQ7KAPR04KHKB";
unsigned long CHANNEL_ID = 865189;

void setup() {
  
  // int prev_time = timeClient.getepochtime();
  pinMode(2,OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32 is initialized");

  //initializing wifi client
  wifiinit();

  //initializing time keeper
  timeClient.begin();
  
  //initializing thingspeak
  //THINGSPEAK TRANSIMISSION RATE IS MINIMUM 15 SECONDS!!!!
  //HAVE TO WAIT FIFTEEN SECONDS BEFORE TRANSMITTING FOR A SECONDS TIME
  ThingSpeak.begin(client);

  //initializing sd card reader
  sdInit();

}

void loop() {

  timeClient.update();

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected....");
    WiFi.reconnect();
  }

  time = timeCLient.getepochtime();

  if(time - prev_time >= INTERVAL){

    float data = get_deformation(1); // 1,2,3 for long, rad, tang; deformation returned in mm
    // float data = get_deformation(SOME_VALUE); //these two for other two dimensions
    // float data = get_deformation(SOME_VALUE);

    prev_time = time;
    status = thingspeak(get_deformation(data), 1);

    if (status != 200){
      println("TRANSMISSION FAILED, LOGGING TO SD.....");
      println("ERROR CODE %d",status);
      logToSD(get_deformation(data));
    }
    else
      Serial.println("THINGSPEAK TRANSMISSION SUCCESFUL.....");
  }

  delay(INTERVAL - time);

}