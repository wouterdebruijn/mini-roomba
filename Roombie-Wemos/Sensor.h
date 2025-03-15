#ifndef SENSOR_h
#define SENSOR_h

#define SENSOR_READ_TIMES 10

#include <Arduino.h>

class Sensor {
  private:
    int emitterPin;
    int sensorPin;
    int ambient;
    int obstacle;
    int value[SENSOR_READ_TIMES];


  public:
    Sensor(int ledPin, int irPin);
    int read();
};

#endif