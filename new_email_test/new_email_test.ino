#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <SD.h>
#include <NTPClient.h> //for date time stamps
#include <HX711.h>     //HX711 by Bogdan Necula
#include <WiFiUdp.h>   //for time stamping
#include <ESP_Mail_Client.h>
#include <time.h>

#define SD_CS_PIN 5

WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
SMTPSession smtp;
Session_Config config;

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "strainguage839@gmail.com"
#define AUTHOR_PASSWORD "vpqthieieiwmsoqn"

#define WIFI_SSID "Purujit" // Corrected SSID
#define WIFI_PASS "PurujitK"

#define RECIPIENT_EMAIL "purujitkantiya@gmail.com"

String name = "";

void setup() {
  Serial.begin(115200);

  wifiinit();  // Connect to Wi-Fi

  // Configure time using NTP
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

  sdInit();  // Initialize SD card
  email_init();  // Initialize email client

  // Get a valid date after sync and create the file
  name = create_file(getFormattedDate());  // Create file with correct date

  logToSD(3.1415, "CAUTION", name);  // Log sample data

  send_email(name);  // Send email with CSV
}



void loop() {
  // No repetitive tasks required here
}

// ==============================
// WiFi Initialization
// ==============================
void wifiinit() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting to WiFi....");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("WiFi not connected...");
  }

  delay(2000);
  Serial.println("Connected to WiFi...");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
  WiFi.mode(WIFI_STA);
}

// ==============================
// SD Card Initialization
// ==============================
void sdInit() {
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    while (1)
      ; // Halt if SD fails
  }

  Serial.println("SD Card initialized.");
}

// ==============================
// Create a New File on SD
// ==============================
String create_file(String formatteddate) {
  String name = "/" + formatteddate + ".csv"; // Corrected to String

  // Create or check strainlog.csv with header
  if (!SD.exists(name)) {
    File file = SD.open(name, FILE_WRITE);
    if (file) {
      file.println("Timestamp,Strain Value,Severity"); // CSV header
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

// ==============================
// Log Data to SD Card
// ==============================
void logToSD(float data, String severity, String name) {
  timeClient.update(); // Ensure time is current

  File file = SD.open(name, FILE_APPEND);

  if (file) {
    file.print(formatted_local_time()); // Correct timestamp format
    file.print(",");
    file.print(data, 2);
    file.print(",");
    file.println(severity); // Add newline at the end
    file.close();
    Serial.println("Data logged to SD as CSV.");
  } else {
    Serial.println("Error opening file: " + name);
  }
}

// ==============================
// Get Formatted Date
// ==============================
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



// ==============================
// Get Formatted Local Time
// ==============================
String formatted_local_time() {
  time_t now;
  time(&now);
  struct tm *timeinfo;
  timeinfo = localtime(&now);

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", timeinfo);
  return String(buffer);
}

// ==============================
// Email Initialization
// ==============================
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

// ==============================
// Send Email with CSV Attachment
// ==============================
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


// ==============================
// SMTP Callback for Debug
// ==============================
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
