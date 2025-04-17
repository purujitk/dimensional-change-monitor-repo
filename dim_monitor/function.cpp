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

String create_file(String formatteddate) {
  String name = "/" + formatteddate + ".csv"; // Corrected to String

  // Create or check strainlog.csv with header
  if (!SD.exists(name)) {
    File file = SD.open(name, FILE_WRITE);
    if (file) {
      file.println("Timestamp,Strain Value"); // CSV header "Timestamp,Strain Value,Severity"
      file.close();
      Serial.println("File created: " + name);
    } else {
      Serial.println("Error creating file: " + name);
    }
  } else {
    Serial.println("File already exists: " + name);
  }

  return name;
}

////////////////MISSED DATE FILE//////////////

// String create_file_missed_date() {
//   String name = "/" + "missed_dates" + ".csv"; // Corrected to String

//   // Create or check strainlog.csv with header
//   if (!SD.exists(name)) {
//     File file = SD.open(name, FILE_WRITE);
//     if (file) {
//       file.println("Missed Dates"); // CSV header
//       file.close();
//       Serial.println("File created: " + name);
//     } else {
//       Serial.println("Error creating file: " + name);
//     }
//   } else {
//     Serial.println("File already exists: " + name);
//   }

//   return name;
// }

///////////logging data to sd card////////////////

void logToSD(float data,/*String severity,*/ String name) {
  timeClient.update(); // Ensure time is current

  File file = SD.open(name, FILE_APPEND);

  if (file) {
    file.print(formatted_local_time()); // Correct timestamp format
    file.print(",");
    file.println(data, 2);
    // file.print(",");
    // file.println(severity); // Add newline at the end
    file.close();
    Serial.println("Data logged to SD as CSV.");
  } else {
    Serial.println("Error opening file: " + name);
  }
}

////////////MISSED DATES/////////////////

// void logToSDmissedDate(String date, String name) {
//   timeClient.update(); // Ensure time is current

//   File file = SD.open(name, FILE_APPEND);

//   if (file) {
//     file.print(date); // Correct timestamp format
//     Serial.println("Data logged to SD as CSV.");
//   } else {
//     Serial.println("Error opening file: " + name);
//   }

// }

/////////////HX711 initialization////////////////

void HX711_init(){

  scale.begin(DATA_LINE,CLOCK_LINE);
  scale.set_gain(128);
  scale.set_scale();
  scale.tare();
  Serial.println("THE Hx711 is calibrated");
  // scale.set_scale(CALIBRATION_FACTOR); really needed?
  
}
//////////////GETTING DEFORMATION////////////////////////

float get_nominal_reading(){
  float nominal = 0;

  Serial.println("starting nominal reading");

  if(scale.is_ready()){

    Serial.println("connected");
    for(int i = 0; i < 10; i++){ //changing to 5 for now change back to 60 later
      nominal += scale.read_average(10);
      delay(500);
    }
  }else
    Serial.println("GUAGE NOT connected");

  
  Serial.println("nominal reading");
  Serial.println(nominal/10,10);
  Serial.println("Nominal reading taken");
  Serial.println((nominal/10) * rawToVoltage_theoretical, 10);

  return ((nominal/10)*rawToVoltage_theoretical);

}

//////////////GETTING DEFORMATION////////////////////////

// 1,2,3 for long, rad, tang; deformation returned in mm (only in the case we use multiple strain guages) --> used with the switch case only


//scale get units returns mass, we need to find the strain

float get_strain(float baseline){

    float strain = 0;
    float new_voltage = 0;
    Serial.println("original strain");
    Serial.println(strain,10);

    Serial.println("this is the baseline value passed in");
    Serial.println(baseline,10);

    if(scale.is_ready()){
      new_voltage = scale.read_average(10)*rawToVoltage_theoretical;
      strain = ((new_voltage - baseline)/EXC_VOLT)*conversion_factor*4/GUAGE_FACTOR;
      Serial.println("new voltage");
      Serial.println(new_voltage,10);
      Serial.println("strain measured");
      Serial.println(strain,10);
    }else
      Serial.println("STRAIN GUAGE NOT CONNECTED");

    return strain;

  }

///////////////////////INITIALIZE EMAIL////////////////////////////

void email_init() {
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

void send_email(String name) {
  SMTP_Message message;
  message.sender.name = "ESP32 CSV Sender";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "CSV Data from ESP32";
  
  message.addRecipient("Recipient", RECIPIENT_EMAIL);

  if (SD.exists(name)) {
    SMTP_Attachment att;
    att.descr.filename = name.c_str();  // Convert String to char*
    att.descr.mime = "text/csv";        // Set the MIME type
    att.file.path = name.c_str();       // File path on SD card
    att.file.storage_type = esp_mail_file_storage_type_sd;  // Use SD card storage

    // Add attachment to the message
    message.addAttachment(att);
  } else {
    Serial.println("CSV file not found on SD card!");
  }

  if (!smtp.connect(&config)) {  // Use &config, not &session
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

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());

  if (status.success()) {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      SMTP_Result result = smtp.sendingResult.getItem(i);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }

    Serial.println("----------------\n");
    smtp.sendingResult.clear();
  }
}

/////////////////////////////////FORMATTED TIME////////////////////////////////////////////

String formatted_local_time() {
  time_t now;
  time(&now);
  struct tm *timeinfo;
  timeinfo = localtime(&now);

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", timeinfo);
  return String(buffer);
}

////////////////////////FORMATED DATE//////////////////////////////////

String getFormattedDate() {
  time_t now;
  struct tm timeInfo;

  time(&now);  // Get current time
  localtime_r(&now, &timeInfo);

  char formattedDate[20];  // Format: YYYY-MM-DD
  snprintf(formattedDate, sizeof(formattedDate), "%04d-%02d-%02d",
           timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);

  return String(formattedDate);
}


////////////////////////check severirty//////////////////////////////

String check_severity(float data, float baseline){

  if((abs(data - baseline)/baseline) >= 0.02){ //max strain of guage is 2 perecent so we cant do ten percent --> we can notify if the strain is at the limit
    return "CAUTION";
  }else
    return "NORMAL";

}
























