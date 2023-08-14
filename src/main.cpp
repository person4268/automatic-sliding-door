#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("hello.");
  digitalWrite(LED_BUILTIN, 1);
  delay(500);
  Serial.print("Oncemore, ");
  digitalWrite(LED_BUILTIN, 0);
  delay(500);
}