#include "Sensor.h"

Sensor::Sensor(uint8_t id)
{
    this->id = id;
}

uint8_t Sensor::getId()
{
    return this->id;
}

uint16_t Sensor::getValue()
{
    return this->value;
}

void Sensor::setValue(uint16_t value)
{
    this->value = value;
}

String Sensor::asJson()
{
    String text = "{\"id\":\"";
    text += String(this->id);
    text += "\",\"value\":\"";
    text += String(this->value);
    text += "\"}";

    return text;
}