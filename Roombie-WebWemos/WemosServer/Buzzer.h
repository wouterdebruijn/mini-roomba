#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

#define BUZZER_ID_MASK 0x30

#define NOTES_L 5

class Buzzer
{
public:
    Buzzer(uint8_t pin, uint8_t id);
    void happy();
    void sad();
    void angry();
    void surprise();
    uint8_t getId();
    uint8_t available();
    void handle();

private:
    uint8_t pin;
    uint8_t id;
    uint16_t notes[NOTES_L] = {0};
    uint8_t currentNote = NOTES_L;
};

#endif // BUZZER_H