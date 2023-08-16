#include <Arduino.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int outlet_pin = 8;

int v_close_pin = 9;
int v_open_pin = 10;
int pv;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(v_close_pin, OUTPUT);
  pinMode(v_open_pin, OUTPUT);  
  pinMode(outlet_pin, INPUT);
  digitalWrite(v_close_pin, LOW);
  digitalWrite(v_open_pin, LOW);
  
  pv = digitalRead(outlet_pin);
}


void pulse(int pin){
  digitalWrite(pin, HIGH);
  delay(300);
  digitalWrite(pin, LOW);
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