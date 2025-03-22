#ifndef WEB_H
#define WEB_H
#include <Arduino.h>

#define BUFFER_SIZE 8

class Web {
  public:
    Web(HardwareSerial *serial);
    HardwareSerial *serial;
    void handle_command();
    bool autoEnabled();
    bool brushesEnabled();

  private:
    char buffer[BUFFER_SIZE] = {};
    int bufferPos = 0;

    bool cmd_auto = false;
    bool cmd_brushes = false;
};

#endif // WEB_H