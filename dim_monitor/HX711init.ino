/*
HX711init.ino

by: purujit Kantiya


HX711 INITIALIZATION SCRIPT ONLY NEEDS TO BE RUN ONCE WITH KNOWN STRAIN

STRAIN CANNOT EXCEED 2000 MICROSTRAIN
*/


#include <function.h>

float hx711_init(){

  scale.begin(DATA_LINE,CLOCK_LINE);
  scale.set_scale();
  scale.tare();

  //place know weight (ie 2kg or sum) within 10 seconds
  delay(10000);

  float known_strain = SOMEVALUE; // calculated using 
  long exp_strain = scale.get_value(10); //from HX711

  return (float) exp_strain/known_strain;


void setup(){
  
  Serial.begin(115200);

  Serial.prinln(hx711_init()); //NOTE THIS VALUE!!


}
