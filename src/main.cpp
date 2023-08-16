#include <Arduino.h>
#include "HX711.h"

int OUTLET_TRIGGER_PIN = 8;

int SOL_CLOSE_PIN = 9;
int SOL_OPEN_PIN = 10;
int currentVal;
int LOADCELL_DOUT_PIN = 2;
int LOADCELL_SCK_PIN = 3;
HX711 scale;

long SCALE_DIVIDER = 1;
long SCALE_OFFSET = 0;

int ERROR_LED = LED_BUILTIN; // for now?

float MAX_SCALE_CHANGE_PER_SECOND = 1.0; // (once calibrated should be kg/s)

void setup() {                
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SOL_CLOSE_PIN, OUTPUT);
  pinMode(SOL_OPEN_PIN, OUTPUT);  
  pinMode(ERROR_LED, OUTPUT);
  pinMode(OUTLET_TRIGGER_PIN, INPUT);

  digitalWrite(SOL_CLOSE_PIN, LOW);
  digitalWrite(SOL_OPEN_PIN, LOW);
  
  currentVal = digitalRead(OUTLET_TRIGGER_PIN);

  // load cell amplifier scaler 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(SCALE_DIVIDER);
  scale.set_offset(SCALE_OFFSET);

}

void calibrate_scale(float known_weight_kg = 1.0) {
  if (scale.is_ready()) {
    // Value the raw scale units gets divided by (defaults to 1)
    Serial.println("Current scale: " + String(scale.get_scale()));
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare(10);
    Serial.println("Tare done..."); // print offset
    Serial.println("Offset was " + String(scale.get_offset()));
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);  // get last 10 readings and average them I think
    Serial.print("Result: ");
    reading = reading / known_weight_kg;
    Serial.print("Result divided by weight: " + String(reading));
    // Setting updated scale based on new reading
    scale.set_scale(scale.get_scale() * reading); // not sure if this is right, but should be
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  Serial.println("Going for another round in 3 seconds...");
  delay(3000);
}


float read_scale() {
  if (scale.is_ready()) return scale.get_units(10);
  else return -1;
}


enum State { DONE, OPENING, CLOSING };

State state = DONE;
unsigned long timeStarted = millis();

float lastScaleValue = 0;
float currentScaleValue = 0;

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
      /* Probably not necessary but in case I accidentally create an edge case
       * (where we transition directly from OPENING->CLOSING), I want to
       * make sure the other solenoid is off */
      digitalWrite(SOL_CLOSE_PIN, LOW);
      if (millis() - timeStarted > 10000) {
        state = DONE;
      }
      break;
    case CLOSING:
      digitalWrite(SOL_OPEN_PIN, LOW);
      digitalWrite(SOL_CLOSE_PIN, HIGH);
      if (millis() - timeStarted > 10000) {
        state = DONE;
      }
      break;
  }

  if (millis() % 20 == 0) { // Every 20ms, update scale values
    lastScaleValue = currentScaleValue;
    currentScaleValue = read_scale();
  }

  /* Triggered if door is either crushing someone or done closing */
  if (state != DONE && currentScaleValue != -1 && lastScaleValue != -1) {
    if (abs(currentScaleValue - lastScaleValue) * 50 > MAX_SCALE_CHANGE_PER_SECOND) {
      state = DONE;
    }
  }

  if (!scale.is_ready()) {
    // Blink error led
    if (millis() % 1000 < 500) { // on for 500ms, off for 500ms (thank you copilot)
      digitalWrite(ERROR_LED, HIGH);
    } else {
      digitalWrite(ERROR_LED, LOW);
    }
  } else {
    digitalWrite(ERROR_LED, LOW);
  }
}