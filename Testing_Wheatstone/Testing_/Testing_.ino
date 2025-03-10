#include <HX711.h>

// Define pins
#define DT 4    // Data pin (DOUT)
#define SCK 16   // Clock pin (SCK)

// Create HX711 object
HX711 scale;

void setup() {



    Serial.begin(115200);  // Start serial monitor
    Serial.println("HX711 Connection Test");

    scale.begin(DT, SCK);  // Initialize HX711

    // Check if HX711 is responding
    if (!scale.is_ready()) {
        Serial.println("ERROR: HX711 not detected. Check wiring!");
        while (1);  // Stop execution if no response
    }
    
    Serial.println("HX711 is connected!");
}

void loop() {
    // Read raw data
    long reading = scale.get_units(10); // Average 10 readings
    Serial.print("Raw Reading: ");
    Serial.println(reading);

    delay(1000); // Wait 1 second
}

