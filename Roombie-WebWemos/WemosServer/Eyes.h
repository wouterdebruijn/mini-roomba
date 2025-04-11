#ifndef EYES_H
#define EYES_H

#include <Arduino.h>

#define EYES_ID_MASK 0x20

class Eyes
{
public:
    Eyes(uint8_t r, uint8_t g, uint8_t b, uint8_t id);
    void set(uint8_t happiness);
    uint8_t getId();

private:
    uint8_t id;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t happiness = 255;
};

#endif