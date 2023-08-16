#include <Arduino.h>
#include "HX711.h"

int led = 13;
int outlet_pin = 8;

int v_close_pin = 9;
int v_open_pin = 10;
int pv;
int LOADCELL_DOUT_PIN = 2;
int LOADCELL_SCK_PIN = 3;
HX711 scale;

void setup() {                
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(v_close_pin, OUTPUT);
  pinMode(v_open_pin, OUTPUT);  
  pinMode(outlet_pin, INPUT);
  digitalWrite(v_close_pin, LOW);
  digitalWrite(v_open_pin, LOW);
  
  pv = digitalRead(outlet_pin);

  // load cell amplifier scaler 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

}


void pulse(int pin){
  digitalWrite(pin, HIGH);
  delay(10000);
  digitalWrite(pin, LOW);
}


void calibrate_scale() {
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);  // get last 10 readings and average them I think
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}


// the loop routine runs over and over again forever:
void loop() {
  int ov = digitalRead(outlet_pin);
  digitalWrite(led, ov);
  
  if(ov && !pv){
    pulse(v_open_pin);
  }
  
  if(pv && !ov){
    pulse(v_close_pin);
  }
  
  
  pv = ov;
}