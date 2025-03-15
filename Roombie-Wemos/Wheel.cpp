#include <Arduino.h>
#include "Wheel.h"

Wheel::Wheel(int int1, int int2, int epp) {
  wheelPinInt1 = int1;
  wheelPinInt2 = int2;
  eepPin = epp;

  active = false;

  pinMode(wheelPinInt1, OUTPUT);
  pinMode(wheelPinInt2, OUTPUT);
  pinMode(eepPin, OUTPUT);

  digitalWrite(eepPin, LOW);
}

void Wheel::forwards() {
  digitalWrite(wheelPinInt1, HIGH);
  digitalWrite(wheelPinInt2, LOW);
}

void Wheel::backwards() {
  digitalWrite(wheelPinInt1, LOW);
  digitalWrite(wheelPinInt2, HIGH);
}

void Wheel::enable() {
  digitalWrite(eepPin, HIGH);
  active = true;
}

void Wheel::disable() {
  digitalWrite(eepPin, LOW);
  active = false;
}

bool Wheel::enabled() {
  return active;
}