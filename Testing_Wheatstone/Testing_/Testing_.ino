#include <HX711.h>

#define DT_PIN 16
#define SCK_PIN 4

HX711 scale;
float gaugeFactor = 2.0;               // Typical GF for strain gauges
float excitationVoltage = 3.3;         // Bridge excitation voltage

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);

  if (scale.is_ready()) {
    Serial.println("HX711 is ready.");
  } else {
    Serial.println("HX711 not detected. Check wiring.");
  }

  scale.set_gain(64);
  scale.set_scale();  // No calibration initially
  scale.tare();       // Zero baseline
  Serial.println("Tare complete. Ready for calibration...");
  
  // Capture baseline reading
  // long baselineReading = scale.read();
  // Serial.print("Baseline ADC Value: ");
  // Serial.println(baselineReading);

  // // Apply known strain and get new reading
  // delay(5000);  // Wait for user to apply strain
  // long strainReading = scale.read();
  // Serial.print("ADC Value with Known Strain: ");
  // Serial.println(strainReading);

  // // Calculate delta ADC
  // long deltaADC = strainReading - baselineReading;
  // Serial.print("Delta ADC: ");
  // Serial.println(deltaADC);

  // // Expected strain and calculation of calibration factor
  // float knownStrain = 200e-6;  // Example: 200 microstrain //NEEDS TO BE CALCULATED USING POINT LOAD ON THE BEAM
  // calibrationFactor = deltaADC / (knownStrain * gaugeFactor);
  // Serial.print("New Calibration Factor: ");
  // Serial.println(calibrationFactor);

  // // Set the new calibration factor
  // scale.set_scale(calibrationFactor);
  // scale.tare();
  // Serial.println("Calibration complete!");
}

void loop() {
  float strainReading = scale.get_units(10);
  Serial.print("Strain (microstrain): ");
  Serial.println(strainReading);
  delay(1000);
}