#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

class Motor
{
public:
  Motor(uint8_t int1, uint8_t int2, uint8_t eep);
  void drive(int16_t speed);
  void state(bool enabled);

private:
  void update();
  int int1Pin;
  int int2Pin;
  int eepPin;
  int speed;
};

#endif // MOTOR_H