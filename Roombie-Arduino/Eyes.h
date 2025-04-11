#ifndef EYES_H
#define EYES_H
#include <Arduino.h>
#include <Servo.h>
#include "Web.h"

#define EYES_ID_MASK 0x20

#define SUPER_HAPPY 255
#define HAPPY 191
#define SAD 127
#define ANGRY 63

class Eyes
{
public:
  Eyes(Web *web, Servo *servo, uint8_t id);
  void setHappiness(uint8_t value);


private:
  Web *web;
  Servo *servo;
  uint8_t id;
};

#endif // EYES_H