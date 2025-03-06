/*
function.c

by: Purujit Kanitya

*/

#include "function.h"

///////////wifi initialization/////////////

void wifiinit(char ssid, char password){

  WiFi.begin(ssid, password);
  Serial.println("connecting to wifi....");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Wifi not connnected...");
  }
  delay(2000);

  Serial.println("connected to the wifi...");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  WiFi.mode(WIFI_STA);

}