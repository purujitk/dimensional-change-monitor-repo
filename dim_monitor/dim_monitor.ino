/*
dim_monitor.ino

by:Purujit Kantiya, Abedal rehman shehabi, Hannah Rogers, Valentine Adeseyie, Gabriella Belanger

Dimensional change monitor main script for APSC 103 project

client: Ingenium
*/

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

  wifiinit();

  timeClient.begin();
  
  ThingSpeak.begin(client);

  int stat = ThingSpeak.writeField(CHANNEL_ID, 1, "HELLO", W_API_KEY);

  if (stat == 200) {
    Serial.println("transmission 1 Succesful, Check Thingspeak.........");
  }

  delay(15000); //THINGSPEAK TRANSIMISSION RATE IS MINIMUM 15 SECONDS!!!!

  int new_stat = ThingSpeak.writeField(CHANNEL_ID, 2, "WORLD", W_API_KEY);

  if (new_stat == 200) {
    Serial.println("transmission 2 Succesful, Check Thingspeak.........");
  } else{
    Serial.println("FAILED");
    Serial.println(new_stat);
  }
}

void loop() {

  timeClient.update();

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected....");
    WiFi.reconnect();
  }
  
  delay(5000);

  // if(timeClient.getepochtime() - prev_time >= INTERVAL){

  //   float data = get_deformation(1);
  //   // float data = get_deformation(SOME_VALUE); //these two for other two dimensions
  //   // float data = get_deformation(SOME_VALUE);

  //   prev_time = timeClient.getepochtime();
  //   status = thingspeak(data);

  //   if (status != 200){
  //     logToSD(data);
  //   }
  // }
  
}