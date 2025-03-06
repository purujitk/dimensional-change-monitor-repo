#include "function.h"


WiFiClient client;

void setup() {

  pinMode(2,OUTPUT);

  //SERIAL MONITOR INITIZALIZATION
  Serial.begin(115200);
  Serial.println("ESP32 is initialized");


  /////////Wifi initialization////////////
  wifiinit(ssid, password);
  
  ////////////////////////////////////////

  ThingSpeak.begin(client); //initializing thingspeak
  int status = ThingSpeak.writeField(channel_id, 1, "hello", w_api_key);

  if (status == 200){
    Serial.println("Transmission succesful...");
  } else {
    Serial.println("Not Connected...");
  }


}

void loop() {

  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi disconnected....");
    WiFi.reconnect();

  }

}