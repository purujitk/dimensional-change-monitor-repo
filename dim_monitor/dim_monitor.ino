#include "function.h"


WiFiClient client;
NTPClient timeClient;

void setup() {
  
  int prev_time;
  int now_time;

  pinMode(2,OUTPUT);

  Serial.begin(115200);
  Serial.println("ESP32 is initialized");

  wifiinit();
  
  ThingSpeak.begin(client);

}

void loop() {

  updatetime()
  now_time = timeClient.getepochtime()

  if(now_time-past_time >= INTERVAL){
    ;// read sensor value
  }