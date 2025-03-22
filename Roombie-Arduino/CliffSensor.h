#ifndef CLIFFSENSOR_H
#define CLIFFSENSOR_H
#include <Arduino.h>

#define IR_TIMES 3

class CliffSensor
{
public:
  CliffSensor(uint8_t ir, uint8_t led, uint8_t id);
  uint16_t read();
  uint16_t getValue();
  uint8_t getId();

private:
  uint8_t id;
  uint8_t led;
  uint8_t ir;
  uint16_t value;
};

#endif // CLIFFSENSOR_H