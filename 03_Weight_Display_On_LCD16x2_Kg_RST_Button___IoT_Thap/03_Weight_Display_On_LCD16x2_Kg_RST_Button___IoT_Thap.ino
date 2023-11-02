#include "HX711.h"
#include <LiquidCrystal.h>

//set calibration factor known weight measurement if you not get perfect weight
#define calibration_factor 98980.994//84//-90220 // for 20kg load cell
//#define calibration_factor -28160 // for 40kg load cell
//#define calibration_factor -208600 // for 10kg load cell


#define LOADCELL_DOUT_PIN  A1
#define LOADCELL_SCK_PIN  A2

//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
float weight;
float real_weight;
float minimum_weight = 0.001; // minimum weight 0.10 kg
float average = 0;


LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
HX711 scale;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("- Object Weight-");
  lcd.setCursor(0, 1);
  lcd.print("in Kg.:");
  // load cell code below
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
}
float weightReadInstance()
{
  //load cell data  weight in kg
  weight = (0.454 * scale.get_units());
  if (weight > minimum_weight) {
    real_weight = weight;
  }
  else {
    real_weight = 0.00;
  }
  return real_weight;
}
void loop() {
  lcd.setCursor(7, 1);
  average = weightReadInstance();
  Serial.println(average, 3);
  lcd.print (average, 3);
  lcd.print (" Kg");
  delay(1000);
}
