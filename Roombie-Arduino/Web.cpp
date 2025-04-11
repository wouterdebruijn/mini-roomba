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

    Serial.print("Sending sensor:");
    Serial.print(sensor->getId());
    Serial.print(", ");
    Serial.println(sensor->getValue());

    uint16_t value = sensor->getValue();

    // Startbit1, Startbit2, AttributeID, 16bit_buffer_part1, 16bit_buffer_part2, Endbit
    uint8_t buffer[] = {128, 0, sensor->getId(), (value & 0xff00) >> 8, value & 0x00ff, 127};

    serial->write(buffer, 6);

    delay(10);
  }

  // End of string
}