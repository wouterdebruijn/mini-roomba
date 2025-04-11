#include "Eyes.h"

Eyes::Eyes(Servo *servo, uint8_t r, uint8_t g, uint8_t b, uint8_t id) : servo(servo), r(r), g(g), b(b), id(EYES_ID_MASK | id)
{
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);

  analogWrite(r, 255);
  analogWrite(g, 0);
  analogWrite(b, 0);
}

void Eyes::set(uint8_t happiness)
{
  uint8_t targetR = 0;
  uint8_t targetG = 0;
  uint8_t targetB = 0;

  if (happiness < 64) {
    // Angry
    targetR = 255;
    servo->write(170);
  } else if (happiness < 128) {
    // Sad
    targetB = 255;
    servo->write(60);
  } else if (happiness < 192) {
    // Happy
    targetG = 255;
    servo->write(95);
  } else {
    // Mega Happy
    targetG = 255;
    servo->write(30);
  }

  analogWrite(r, targetR);
  analogWrite(g, targetG);
  analogWrite(b, targetB);
}

uint8_t Eyes::getId() {
  return this->id;
}