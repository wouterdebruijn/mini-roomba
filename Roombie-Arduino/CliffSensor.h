#ifndef CLIFFSENSOR_H
#define CLIFFSENSOR_H
#include <Arduino.h>

#define IR_TIMES  3

class CliffSensor {
  public:
    CliffSensor(uint8_t ir, uint8_t led);
    uint16_t read();

  private:
    uint8_t led;
    uint8_t ir;
};

#endif // CLIFFSENSOR_H