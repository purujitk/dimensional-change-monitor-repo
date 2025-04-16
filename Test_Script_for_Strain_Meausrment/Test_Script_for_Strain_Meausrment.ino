#include <HX711.h>

#define DT_PIN 16
#define SCK_PIN 4

HX711 scale;

float gauge_factor = 2.0;
float exc_volt = 2.24;

float strain = 0;

float conversion_factor = 1000000;

const float Vref   = 1.25;

const float Vfs = Vref/128;

const long steps = 8388608;    // 2^23
float rawToVoltage_theoretical = Vfs / steps;

/* strain conversion formula

microstrain = (4/gauge_factor)*Bout

Bout = Vout/Vex

*/

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);

  while(!scale.is_ready()){
    Serial.println("hx711 not connected");
  }

  Serial.println("connected");    
  scale.set_gain(128);

  scale.tare();
  Serial.println("Tare complete. Ready for calibration...");

  float nominal =0;
  float nom_volt_br = 0;

  Serial.println("starting nominal reading");

  for(int i = 0; i < 10; i++){
    nominal += scale.read_average(10);
    delay(500);
  } 

  Serial.println("Nominal reading taken");

  nom_volt_br = nominal/10*rawToVoltage_theoretical;

  Serial.println(nom_volt_br,10);

  Serial.println("place bag");
  for(int i = 0; i < 10; i++){
    Serial.println((i+1));
    Serial.println(" seconds....");
    delay(1000);
  }


  Serial.println("getting strain");

  if(scale.is_ready()){
    Serial.println("new voltage");

    float new_voltage = (scale.read_average(10))*rawToVoltage_theoretical;

    Serial.println(new_voltage,10);

    float strain = ((new_voltage-nom_volt_br)/exc_volt)*conversion_factor*4/gauge_factor;

    Serial.println("strain reading complete");

    Serial.println(strain,20);
  }


}

void loop() {
  // put your main code here, to run repeatedly:

}
