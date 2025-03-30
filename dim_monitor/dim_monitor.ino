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
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); //MIGHT NOT BE NEEDED CAN JUST USE CONFIG TIME
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
float data =0;

const float Vref   = 1.25;    // Internal reference voltage (typical)
const float Gain   = 64.0;   // Amplifier gain for channel A
const float Vfs = Vref / Gain; // e.g., 1.25 / 128 = ~0.00977 
const long steps = 8388608;
float rawToVoltage_theoretical = Vfs / steps;


void setup() {
  
  //initializations

  Serial.begin(115200);
  
  delay(500);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  //for sleep wakup --> want to skip the setup after wakeup
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Woke up from deep sleep. Skipping setup...");
    return;  // Skip rest of setup if woke up from sleep
  }else 
    Serial.println(wakeup_reason);

  //initialize wifi
  wifiinit();

  //initialize the time
  configTime(-18000, 3600, "pool.ntp.org", "time.nist.gov");

  Serial.println("Fetching time...");
  delay(2000);

  // Wait until a valid time is obtained
  time_t now;
  time(&now);
  while (now < 1700000000) {  // 1700000000 is around 2023, ensures time is valid
    Serial.println("Waiting for NTP sync...");
    delay(500);
    time(&now);
  }

  Serial.println("Time synchronized successfully!");

  //initialize sd card
  sdInit();

  //initialize email
  email_init();

  //initialize the hx711
  HX711_init();
  delay(2000);
  if(scale.is_ready()){
    Serial.println("hx711 connected");
  }

  //getting baseline value
  baseline = get_nominal_reading();

  //getting inital date
  date = getFormattedDate();

  //creating initial file
  name = create_file(date);


}

void loop() {

  timeClient.update();

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("not connetced to wifi");
    for(int i = 0; i < 3; i++){
      WiFi.reconnect();
    }
  }

  time_dif = (timeClient.getEpochTime() - prev_time);
  Serial.println("time dif is ");
  Serial.println(time_dif);

  if (time_dif >= 60){

    prev_time = timeClient.getEpochTime();

    data = get_strain(baseline);

    if(/*getFormattedDate()*/ String("2006-12-14") != date){

      logToSD(data, check_severity(data, baseline), name);

      if(WiFi.status() == WL_CONNECTED){
        Serial.println("wifi connected for email");
        send_email(name);
      }else
        //we can store the date that could not be sent in an array possibly and then check to send that data later

      date = getFormattedDate();
      name = create_file(date);

    }
      


  }else
    //sleep for some time
    Serial.println("sleep section");
    esp_sleep_enable_timer_wakeup(60*1000000); // --> should set this to the intrval size
    esp_deep_sleep_start();
    Serial.println("out of sleep");

}

