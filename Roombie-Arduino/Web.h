#ifndef WEB_H
#define WEB_H
#include <Arduino.h>
#include "CliffSensor.h"

#define BUFFER_SIZE 8

class Web
{
public:
  Web(HardwareSerial *serial, CliffSensor **sensors);
  HardwareSerial *serial;
  void handle_command();
  void sendMetrics();
  void sendValue(uint8_t id, uint16_t value);
  bool autoEnabled();
  bool brushesEnabled();

private:
  char buffer[BUFFER_SIZE] = {};
  int bufferPos = 0;

  bool cmd_auto = false;
  bool cmd_brushes = true;

  CliffSensor **sensors;
};

#endif // WEB_H