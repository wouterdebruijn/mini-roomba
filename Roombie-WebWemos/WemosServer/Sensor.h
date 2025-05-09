#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

#define SENSOR_ID_MASK 0x00

class Sensor
{
public:
    Sensor(uint8_t id);
    uint8_t getId();
    uint16_t getValue();
    void setValue(uint16_t value);
    String asJson();

private:
    uint8_t id;
    uint16_t value = 0;
};

#endif