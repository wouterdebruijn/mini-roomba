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
  uint16_t reflected = 0;
  int result = 0;

  for (int i = 0; i < IR_TIMES; i++)
  {
    // Disable IR LED and read ambient IR from sensor
    digitalWrite(led, LOW);
    delay(2);
    ambient = 1024 - analogRead(ir);

    // Enable IR LED and read reflected IR
    digitalWrite(led, HIGH);
    delay(2);

    reflected = 1024 - analogRead(ir);
    result += (reflected - ambient);
    digitalWrite(led, LOW);
  }

  uint16_t newValue = 0;

  if (result != 0) {
    newValue = result / IR_TIMES;
  }

  this->changed = this->value < (newValue - 50) || this->value > (newValue + 50);
  this->value = newValue;

  // Return avg
  return this->value;
}

uint16_t CliffSensor::getValue()
{
  return this->value;
}

uint8_t CliffSensor::getId()
{
  return this->id;
}

bool CliffSensor::hasChanged()
{
  return this->changed;
}