#include <HX711.h>

#define DATA_LINE 16
#define CLOCK_LINE 4

float gaugeFactor = 2.0;               // Typical GF for strain gauges
float excitationVoltage = 3.3;         // Bridge excitation voltage // WE HAVE 5 VOLTS

HX711 scale;

void setup(){
  Serial.begin(115600);//check baud rate
  
  scale.begin(DATA_LINE,CLOCK_LINE);  

  scale.set_gain(64);

  HX711Calibration();
}


void HX711Calibration() {

  if (scale.is_ready()) {
    Serial.println("HX711 is ready.");
  } else {
    Serial.println("HX711 not detected. Check wiring.");
    while (1);
  }

  scale.set_scale();  // No calibration initially
  scale.tare();       // Zero baseline
  Serial.println("Tare complete. Ready for calibration...");
  
  // Capture baseline reading
  long baselineReading = scale.read();
  Serial.print("Baseline ADC Value: ");
  Serial.println(baselineReading);

  // Apply known strain and get new reading
  delay(5000);  // Wait for user to apply strain // WE SHOULD INCREASE THE TIME 5 SECONDS NOT ALOT
  long strainReading = scale.read();
  Serial.print("ADC Value with Known Strain: ");
  Serial.println(strainReading);

  // Calculate delta ADC
  long deltaADC = strainReading - baselineReading;
  Serial.print("Delta ADC: ");
  Serial.println(deltaADC);

  // Expected strain and calculation of calibration factor
  float knownStrain = 200e-6;  // Example: 200 microstrain //NEEDS TO BE CALCULATED USING POINT LOAD ON THE BEAM
  calibrationFactor = deltaADC / (knownStrain * gaugeFactor);
  Serial.print("New Calibration Factor: ");
  Serial.println(calibrationFactor);
  Serial.println("^^^^NOTE THIS VALUE DOWN^^^^");

  // Set the new calibration factor
  Serial.println("calibration complete!");
}
