/*
function.cpp

by: Purujit Kanitya

*/

#include "function.h"

///////////wifi initialization/////////////

void wifiinit(){

  WiFi.begin(WIFI_SSD, WIFI_PASS);
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

///////////thingspeak transmission///////////// PROBABLY NOT GOING TO USE NOW WE JUST WANT TO SEND TO EMAIL ERIN

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

}

/////////////////creating sd card file///////////////////

String create_file(String formatteddate){

  char name = "/" + formatteddate + ".csv";

  // Create or check strainlog.csv with header
  if (!SD.exists(name)) { //JUST CHANGE TO FORMATTED TIME IN STEAD OF THAT NAME
    File file = SD.open(name, FILE_WRITE);
    if (file) {
      file.println("Timestamp,Strain Value,Severity"); // CSV header
      file.close();
      Serial.println("file created");
    } else {
      Serial.println("Error creating strainlog.csv");
    }
  }else 
    Serial.println("file already there");

  return name;

}

///////////logging data to sd card////////////////

void logToSD(float data,String severity, String name) {

  timeClient.update(); // Ensure time is current

  File file = SD.open(name, FILE_APPEND);

  if (file) {
    file.print(formatted_local_time()); // e.g., 2025-03-05T12:00:00Z
    file.print(",");
    file.print(data, 2);
    file.print(",")
    file.print(severity); // --> here we want to check wether the strain is severe
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
  Serial.println("THE Hx711 is calibrated");
  // scale.set_scale(CALIBRATION_FACTOR); really needed?
  
}
//////////////GETTING DEFORMATION////////////////////////

float get_nominal_reading(){
  float nominal = 0;
  Serial.println("starting nominal reading");
  for(int i = 0; i < 60; i++){
    nominal += scale.read_average(10);
    delay(1000);
  } 
  
  Serial.println("nominal reading");
  Serial.println(nominal/60);
  Serial.println("Nominal reading taken");
  
  return (nominal/60);

}

//////////////GETTING DEFORMATION////////////////////////


// 1,2,3 for long, rad, tang; deformation returned in mm (only in the case we use multiple strain guages) --> used with the switch case only


//scale get units returns mass, we need to find the strain

float get_strain(float baseline){

    float strain = 0;

    if(scale.is_ready()){
      strain = (scale.read_average(10) - baseline)/(64*GUAGE_FACTOR*EXC_VOLT);
      return strain;
    }else
      Serial.println("STRAIN GUAGE NOT CONNECTED");

  }

///////////////////////INITIALIZE EMAIL////////////////////////////

void email_init(){
  MailClient.networkReconnect(true);
  smtp.debug(1);
  smtp.callback(smtpCallback);
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;
}

//////////////////////EMAIL SENDING////////////////////////////////

void send_email(String name){
  
  SMTP_Message message;
  message.sender.name = "ESP32 CSV Sender";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "CSV Data from ESP32";
  message.addRecipient("Recipient", RECIPIENT_EMAIL);

  if (SD.exists(name)) {
    message.addAttachFile(name, "text/csv");
  } else {
    Serial.println("CSV file not found on SD card!");
  }
  

  if (!smtp.connect(&session)) {
    Serial.println("Failed to connect to SMTP server");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending email: " + smtp.errorReason());
  } else {
    Serial.println("Email sent successfully!");
  }
}



//////////////////////GETTING EMAIL STATUS///////////////////////////////

void smtpCallback(SMTP_Status status){ //refer to random nerds site was taking from there 

  Serial.println(status.info());

  if (status.success()){//sending status
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)
      
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow up.
    smtp.sendingResult.clear();
  }
}

/////////////////////////////////FORMATTED TIME////////////////////////////////////////////

String getFormattedTime() {
  unsigned long epochTime = timeClient.getEpochTime();  // Get the epoch time
  struct tm* timeInfo = localtime(&epochTime);  // Convert epoch time to struct tm

  // Format the time into HH:MM:SS
  char formattedTime[9];  // HH:MM:SS + null terminator
  snprintf(formattedTime, sizeof(formattedTime), "%02d:%02d:%02d", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

  return String(formattedTime);  // Return the formatted time as a String
}

////////////////////////FORMATED DATE//////////////////////////////////

String getFormattedDate() {
  unsigned long epochTime = timeClient.getEpochTime();  // Get the epoch time
  struct tm* timeInfo = localtime(&epochTime);  // Convert epoch time to struct tm

  // Format the date into YYYY-MM-DD
  char formattedDate[11];  // YYYY-MM-DD + null terminator
  snprintf(formattedDate, sizeof(formattedDate), "%04d-%02d-%02d", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

  return String(formattedDate);  // Return the formatted date as a String
}


////////////////////////check severirty//////////////////////////////

String check_severity(float data, float baseline){

  float baseline_strain = baseline/(64*GUAGE_FACTOR*EXC_VOLT);

  if((abs(data - baseline_strain)/baseline_strain) >= 0.02){ //max strain of guage is 2 perecent so we cant do ten percent --> we can notify if the strain is at the limit
    return "CAUTION";
  }else
    return "NORMAL"

}
























