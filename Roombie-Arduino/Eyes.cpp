#include "Eyes.h"

Eyes::Eyes(Web *web, Servo *servo, uint8_t id) : web(web), servo(servo), id(id) {}

void Eyes::setHappiness(uint8_t value)
{
  this->web->sendValue(EYES_ID_MASK | this->id, value);
}

void Eyes::setBuzzer(uint8_t sound)
{
  this->web->sendValue(BUZZER_ID_MASK | this->id, sound);
}

void Eyes::blink(uint8_t count)
{
  this->web->sendValue(EYES_ID_MASK | this->id, count); // hardcoded value for blink trigger
}