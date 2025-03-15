// Wheel.h
#ifndef WHEEL_h
#define WHEEL_h

#include <Arduino.h>

class Wheel {
  private:
    int wheelPinInt1;
    int wheelPinInt2;
    int eepPin;
    bool active;


  public:
    Wheel(int int1, int int2, int epp);
    void forwards();
    void backwards();
    void enable();
    void disable();
    bool enabled();
};

#endif