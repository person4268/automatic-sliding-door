#include <Arduino.h>
#include "HX711.h"

int OUTLET_TRIGGER_PIN = 8;

int SOL_CLOSE_PIN = 9;
int SOL_OPEN_PIN = 10;
int currentVal;
int LOADCELL_DOUT_PIN = 2;
int LOADCELL_SCK_PIN = 3;
HX711 scale;

void setup() {                
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SOL_CLOSE_PIN, OUTPUT);
  pinMode(SOL_OPEN_PIN, OUTPUT);  
  pinMode(OUTLET_TRIGGER_PIN, INPUT);
  digitalWrite(SOL_CLOSE_PIN, LOW);
  digitalWrite(SOL_OPEN_PIN, LOW);
  
  currentVal = digitalRead(OUTLET_TRIGGER_PIN);

  // load cell amplifier scaler 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

}

void calibrate_scale(float known_weight_kg = 1.0) {
  if (scale.is_ready()) {
    // Value the raw scale units gets divided by (defaults to 1)
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


enum State { DONE, OPENING, CLOSING };

State state = DONE;
unsigned long timeStarted = millis();

// the loop routine runs over and over again forever:
void loop() {

  // just a simple edge trigger setup
  int lastVal = digitalRead(OUTLET_TRIGGER_PIN);
  digitalWrite(LED_BUILTIN, lastVal);
  
  if (lastVal && !currentVal) { // if the trigger pin turns on, open door
    state = OPENING;
  }
  
  if (!lastVal && currentVal) { // if off, close door
    state = CLOSING;
  }
  
  currentVal = lastVal;
  switch (state) {
    case DONE:
      digitalWrite(SOL_OPEN_PIN, LOW);
      digitalWrite(SOL_CLOSE_PIN, LOW);
      timeStarted = millis();
      break;
    case OPENING:
      digitalWrite(SOL_OPEN_PIN, HIGH);
      if (millis() - timeStarted > 10000) {
        state = DONE;
      }
      break;
    case CLOSING:
      digitalWrite(SOL_CLOSE_PIN, HIGH);
      if (millis() - timeStarted > 10000) {
        state = DONE;
      }
      break;
  }

  if (state != DONE) {
    // todo: read load cell, and set state to DONE if the rate of change is too high
  }
}