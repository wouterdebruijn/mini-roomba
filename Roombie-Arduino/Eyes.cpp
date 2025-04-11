#include "Eyes.h"

Eyes::Eyes(Web *web, Servo *servo, uint8_t id) : web(web), servo(servo), id(EYES_ID_MASK | id) {}

void Eyes::setHappiness(uint8_t value) {
  this->web->sendValue(this->id, value);
}