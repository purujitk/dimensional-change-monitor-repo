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

unsigned long prev_time = 0;
long baseline = 0;


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

  //initializing the ADC
  HX711_init();

  //getting baseline value (nominal)
  baseline = get_nominal_reading();




}

void loop() {

  timeClient.update();

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected....");
    // WiFi.reconnect();
  }

  if (millis() - prev_time >= INTERVAL) {
    prev_time = millis();  // Update prev_time

    // Take sensor reading and log data
    float data = get_deformation(baseline);
    int status = thingspeaktransmit(data, 1);

    if (status != 200){
      Serial.println("TRANSMISSION FAILED, LOGGING TO SD.....");
      Serial.println("ERROR CODE:");
      Serial.println(status);
      logToSD(data);
    }else
      Serial.println("THINGSPEAK TRANSMISSION SUCCESFUL.....");
  }
  
}

