/*
function.cpp

by: Purujit Kanitya

*/

#include "function.h"

///////////wifi initialization/////////////

void wifiinit(){

  WiFi.begin(WIFI_SSD`  , WIFI_PASS);
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

///////////thingspeak transmission/////////////

int thingspeaktransmit(int sensordata, int field){

  int status = ThingSpeak.writeField(CHANNEL_ID, field, sensordata, W_API_KEY);

  return status;
}

//////////sd card reader initialization//////////////

void sdInit() {

  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    while (1); // Halt if SD fails
  }

  Serial.println("SD Card initialized.");

  // Create or check strainlog.csv with header
  if (!SD.exists("/strainlog.csv")) {
    File file = SD.open("/strainlog.csv", FILE_WRITE);
    if (file) {
      file.println("Timestamp,Strain Value"); // CSV header
      file.close();
      Serial.println("Created strainlog.csv with header");
    } else {
      Serial.println("Error creating strainlog.csv");
    }
  }
}


///////////logging data to sd card////////////////

void logToSD(float strain) {

  timeClient.update(); // Ensure time is current

  File file = SD.open("/strainlog.csv", FILE_APPEND);

  if (file) {
    file.print(timeClient.getFormattedDate()); // e.g., 2025-03-05T12:00:00Z
    file.print(",");
    file.print(strain, 2);
    file.close(); 
    Serial.println("Data logged to SD as CSV.");
  } else {
    Serial.println("Error opening strainlog.csv");
  }

}

/////////////HX711 initialization////////////////

void HX711_init(){

  scale.begin(DATA_LINE,CLOCK_LINE);
  scale.set_gain(64);
  scale.set_scale();
  scale.tare();
  // scale.set_scale(CALIBRATION_FACTOR); really needed?
  
}

//////////////GETTING DEFORMATION////////////////////////


// 1,2,3 for long, rad, tang; deformation returned in mm (only in the case we use multiple strain guages


//scale get units returns mass, we need to find the strain

float get_deformation(){

    if(scale.is_ready()){
      
    }else
      Serial.println("STRAIN GUAGE NOT CONNECTED");

    
  }

}

