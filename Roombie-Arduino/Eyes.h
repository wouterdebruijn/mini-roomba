#ifndef EYES_H
#define EYES_H
#include <Arduino.h>
#include <Servo.h>
#include "Web.h"

#define EYES_ID_MASK 0x20
#define BUZZER_ID_MASK 0x30

#define SUPER_HAPPY 255
#define HAPPY 191
#define SAD 127
#define ANGRY 63

#define SOUND_HAPPY 1
#define SOUND_SAD 2
#define SOUND_SURPRISE 3
#define SOUND_ANGRY 4

class Eyes
{
public:
  Eyes(Web *web, Servo *servo, uint8_t id);
  void setHappiness(uint8_t value);
  void setBuzzer(uint8_t sound);
  void blink(uint8_t count);


private:
  Web *web;
  Servo *servo;
  uint8_t id;
};

#endif // EYES_H