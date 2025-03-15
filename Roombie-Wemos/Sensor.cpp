#include <Arduino.h>
#include "Sensor.h"



Sensor::Sensor(int ledEmitterPin, int irPhotodiodePin) {
  emitterPin = ledEmitterPin;
  sensorPin = irPhotodiodePin;

  pinMode(irPhotodiodePin, INPUT);
  pinMode(ledEmitterPin, OUTPUT);
  digitalWrite(ledEmitterPin, LOW);
}

int Sensor::read() {
  for (int i=0; i++; i < SENSOR_READ_TIMES) {
    digitalWrite(emitterPin, LOW);
    delay(1);
    ambient = analogRead(sensorPin);

    digitalWrite(emitterPin, HIGH);
    delay(1);
    obstacle = analogRead(sensorPin);
    value[i] = ambient - obstacle;
  }

  int distance = 0;

  for (int i=0; i++; i < SENSOR_READ_TIMES) {
    distance+=value[i];
  }

  return distance / SENSOR_READ_TIMES;
}