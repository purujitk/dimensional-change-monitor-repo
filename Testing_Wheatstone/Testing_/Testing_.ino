#include <HX711.h>

#define DT_PIN 16
#define SCK_PIN 4

HX711 scale;

float gaugeFactor = 2.0;               // Typical GF for strain gauges
float excitationVoltage = 3.3;  

float strain = 0;
//changing raw value into strain value
const float Vref   = 1.25;    // Internal reference voltage (typical)
const float Gain   = 64.0;   // Amplifier gain for channel A
  
// Calculate theoretical full-scale differential voltage:
const float Vfs = Vref / Gain; // e.g., 1.25 / 128 = ~0.00977 

// The number of steps in the positive range of a 24-bit ADC (signed)
const long steps = 8388608;    // 2^23
float rawToVoltage_theoretical = Vfs / steps;
// Bridge excitation voltage

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);

  while(!scale.is_ready()){
    Serial.println("hx711 not connected");
  }

  Serial.println("connected");
  
  scale.set_gain(64);
  // scale.set_scale();  // No calibration initially
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
  for(int x = 0; x <10; x++){
    float nominal = 0;

    Serial.println("starting nominal reading");

    for(int i = 0; i < 10; i++){
      nominal += scale.read_average(10);
      delay(500);
    } 
    
    Serial.println("nominal reading");
    Serial.println(nominal/10,10);
    Serial.println("Nominal reading taken");
    
    nominal = (nominal/10)*rawToVoltage_theoretical;

    Serial.println(nominal,15);

    delay(3000);
  }
}

void loop() {
  // float strainReading = scale.read_average(10);
  // Serial.print("RAW: ");
  // Serial.println(strainReading);


  // // strain = ((scale.read_average(10)*rawToVoltage_theoretical) - baseline)/(64*GUAGE_FACTOR*EXC_VOLT);

  // strain = scale.read_average(10)*rawToVoltage_theoretical;

  // Serial.println("strain reading: ");
  // Serial.println(strain);

  // delay(1000);
}