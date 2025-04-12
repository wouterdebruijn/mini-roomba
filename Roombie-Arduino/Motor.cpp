#include "Motor.h"

Motor::Motor(uint8_t int1, uint8_t int2, uint8_t eep)
{
  int1Pin = int1;
  int2Pin = int2;
  eepPin = eep;

  pinMode(int1, OUTPUT);
  pinMode(int2, OUTPUT);
  pinMode(eepPin, OUTPUT);

  digitalWrite(eepPin, LOW);
}

void Motor::drive(int16_t speed)
{
  this->speed = speed;
  this->update();
}

void Motor::state(bool enabled)
{
  digitalWrite(eepPin, enabled);
}

void Motor::update()
{
  bool forward = speed >= 0;

  if (speed == 0)
  {
    analogWrite(int1Pin, 0);
    analogWrite(int2Pin, 0);
    return;
  }

  if (forward)
  {
    analogWrite(int1Pin, speed);
    analogWrite(int2Pin, 0);
  }
  else
  {
    analogWrite(int1Pin, 0);
    analogWrite(int2Pin, speed * -1);
  }
}