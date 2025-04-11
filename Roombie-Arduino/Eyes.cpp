#include "Eyes.h"

Eyes::Eyes(Web *web, Servo *servo, uint8_t id) : web(web), servo(servo), id(EYES_ID_MASK | id) {}

void Eyes::setHappiness(uint8_t value) {
  this->web->sendValue(this->id, value);

  if (value < 64) {
    // Angry
    servo->write(105);
  } else if (value < 128) {
    // Sad
    servo->write(40);
  } else if (value < 192) {
    // Happy
    servo->write(65);
  } else {
    // Mega Happy
    servo->write(30);
  }

  uint8_t servoValue = map(value, 255, 0, 10, 102);
  
}