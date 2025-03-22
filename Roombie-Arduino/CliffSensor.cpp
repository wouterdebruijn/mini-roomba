#include "CliffSensor.h"

CliffSensor::CliffSensor(uint8_t ir, uint8_t led, uint8_t id) : ir(ir), led(led)
{
  pinMode(led, OUTPUT);
  pinMode(ir, INPUT_PULLUP);

  this->id = SENSOR_ID_MASK | id;
}

uint16_t CliffSensor::read()
{
  uint16_t ambient = 0;
  uint16_t result = 0;

  for (int i = 0; i < IR_TIMES; i++)
  {
    digitalWrite(led, LOW);
    delay(5);
    ambient = analogRead(ir);
    digitalWrite(led, HIGH);
    result += ambient - analogRead(ir);
    digitalWrite(led, LOW);
  }

  uint16_t value = result / IR_TIMES;

  this->changed = this->value < (value - 50) || this->value > (value + 50);
  this->value = value;

  // Return avg
  return this->value;
}

uint16_t CliffSensor::getValue() {
  return this->value;
}

uint8_t CliffSensor::getId() {
  return this->id;
}

bool CliffSensor::hasChanged() {
  return this->changed;
}