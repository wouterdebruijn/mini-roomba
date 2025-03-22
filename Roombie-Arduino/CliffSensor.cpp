#include "CliffSensor.h"

CliffSensor::CliffSensor(uint8_t ir, uint8_t led): ir(ir), led(led) {
  pinMode(led, OUTPUT);
  pinMode(ir, INPUT_PULLUP);
}

uint16_t CliffSensor::read() {
  int ambient = 0;
  int result = 0;

  for (int i=0; i < IR_TIMES; i++) {
    digitalWrite(led, LOW);
    delay(5);
    ambient = analogRead(ir);
    digitalWrite(led, HIGH);
    result += ambient - analogRead(ir);
    digitalWrite(led, LOW);
  }

  // Return avg
  return result / IR_TIMES;
}