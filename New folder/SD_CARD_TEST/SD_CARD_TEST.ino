#include <SPI.h>  // Add this!
#include "SD.h"

//SD CARD MODULE REQUIRES 5 V

#define SD_CS 5  // Default CS pin, change if needed

void setup() {
    Serial.begin(115200);
    
    // Manually initialize SPI before calling SD.begin()
    SPI.begin(18, 19, 23, SD_CS);  // SCK, MISO, MOSI, CS
    Serial.println("SPI bus initialized...");

    if (!SD.begin(SD_CS)) {
        Serial.println("Card Mount Failed");
        return;
    }

    Serial.println("SD Card initialized successfully!");

    // Simple file write test
    writeFile(SD, "/test.txt", "Hello from ESP32 10-0 Purujit Kantiya\!\n");
    readFile(SD, "/test.txt");
}

void loop() {
    // Nothing here for now
}

// Function to write to a file
void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing to file: %s\n", path);
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written successfully");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

// Function to read from a file
void readFile(fs::FS &fs, const char *path) {
    Serial.printf("Reading file: %s\n", path);
    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("File content:");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}
