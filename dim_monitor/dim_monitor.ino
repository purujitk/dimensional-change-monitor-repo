/*
dim_monitor.ino

by:Purujit Kantiya, Abedal rehman shehabi, Hannah Rogers, Valentine Adeseyie, Gabriella Belanger

Dimensional change monitor main script for APSC 103 project

client: Ingenium
*/

#include "function.h"


WiFiClient client;
NTPClient timeClient;
HX711 scale;


void setup() {
  
  int prev_time = timeClient.getepochtime();

  pinMode(2,OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32 is initialized");

  wifiinit();

  timeClient.begin()
  
  ThingSpeak.begin(client);

}

void loop() {

  timeClient.update();

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected....");
    WiFi.reconnect()
  }

  if(timeClient.getepochtime() - prev_time >= INTERVAL){

    float data = get_deformation(1);
    // float data = get_deformation(SOME_VALUE); //these two for other two dimensions
    // float data = get_deformation(SOME_VALUE);

    prev_time = timeClient.getepochtime();
    status = thingspeak(data);

    if (status != 200){
      logToSD(data);
    }
  }
  
}