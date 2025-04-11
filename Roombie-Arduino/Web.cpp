#include "Web.h"

Web::Web(HardwareSerial *serial, CliffSensor **sensors)
    : serial(serial), sensors(sensors)
{
}

bool Web::autoEnabled()
{
  return cmd_auto;
}

bool Web::brushesEnabled()
{
  return cmd_brushes;
}

void Web::handle_command()
{
  if (serial->available() > 0 && bufferPos < BUFFER_SIZE)
  {
    // Read one byte to buffer
    buffer[bufferPos] = serial->read();

    // Add length to buffer
    bufferPos++;
  }
  else if (bufferPos > 0)
  {
    if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '2')
    {
      // cmd:2, toggle auto
      cmd_auto = !cmd_auto;
      bufferPos = 0;
    }

    if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '3')
    {
      // cmd:2, toggle auto
      cmd_brushes = !cmd_brushes;
      bufferPos = 0;
    }
  }
}

void Web::sendValue(uint8_t id, uint16_t value)
{
  // Startbyte1, AttributeID, 16bit_buffer_part1, 16bit_buffer_part2, Endbyte1, Endbyte2
  uint8_t buffer[] = {0, id, (value & 0xff00) >> 8, value & 0x00ff, 127, 129};
  serial->write(buffer, 6);
}

void Web::sendMetrics()
{
  uint8_t i = 0;
  for (int i = 0; sensors[i] != nullptr; i++)
  {
    CliffSensor *sensor = sensors[i];

    if (!sensor->hasChanged())
    {
      continue;
    }

    this->sendValue(sensor->getId(), sensor->getValue());

    delay(10);
  }

  // End of string
}