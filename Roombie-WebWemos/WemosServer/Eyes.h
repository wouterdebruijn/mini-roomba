#ifndef EYES_H
#define EYES_H

#include <Arduino.h>

class Eyes
{
public:
    Eyes(uint8_t r, uint8_t g, uint8_t b);
    void set(uint8_t happiness);

private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t happiness = 255;
};

#endif