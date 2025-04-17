/*
dim_monitor.ino

by:Purujit Kantiya, Abedal rehman shehabi, Hannah Rogers, Valentine Adeseyie, Gabriella Belanger

Dimensional change monitor main script for APSC 103 project

client: Ingenium
*/
#include "function.h"
#include <esp_sleep.h>


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
RTC_DATA_ATTR bool has_initialized = false;
RTC_DATA_ATTR float baseline = 0;

unsigned long time_dif = 0;

RTC_DATA_ATTR String date = "";
RTC_DATA_ATTR String name = "";
float data = 0;

const float Vref   = 1.25;    // Internal reference voltage (typical)
const float Gain   = 128.0;   // Amplifier gain for channel A
const float Vfs = Vref / Gain; // e.g., 1.25 / 128 = ~0.00977 
const long steps = 8388608;
float rawToVoltage_theoretical = Vfs / steps;

RTC_DATA_ATTR float baseline_strain;

bool date_missed = false;


void setup() {
  //initializations
  Serial.begin(115200);
  delay(500);

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

  //getting baseline voltage reading value
  baseline = get_nominal_reading();

  //converting baseline voltage to baseline strain
  baseline_strain = (baseline/EXC_VOLT)*conversion_factor*4/GUAGE_FACTOR;
  Serial.println("this is the baseline strain");
  Serial.println(baseline_strain,10);

  //getting inital date
  date = getFormattedDate();

  //creating initial file
  name = create_file(date);

  //create missed date file
  // create_file_missed_date();

  //sleep timer
  esp_sleep_enable_timer_wakeup(1800*1000000);


}

void loop(){

  timeClient.update();

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("not connetced to wifi");
    for(int i = 0; i < 10; i++){
      WiFi.reconnect();
      delay(100);
    }
  }

  data = get_strain(baseline);

  logToSD(data, name);

  if(getFormattedDate() != date){ //String("2006-12-14") // this need to be changed to get_formatteddat(); so that email can be sent when the on new day

    logToSD(data,/* check_severity(data, baseline_strain),*/ name);

    if(WiFi.status() == WL_CONNECTED){
      Serial.println("wifi connected for email");
      send_email(name);
    }

    date = getFormattedDate();
    name = create_file(date);

  }
  
  Serial.println("going in to light sleep");

  delay(1000);

  esp_light_sleep_start(); //should take a 60 second gap

  Serial.println("woken up... starting loop again");


}




