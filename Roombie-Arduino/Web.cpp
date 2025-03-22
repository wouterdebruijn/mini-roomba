#include "Web.h"

Web::Web(HardwareSerial *serial) : serial(serial) {
}

bool Web::autoEnabled() {
  return cmd_auto;
}

bool Web::brushesEnabled() {
  return cmd_brushes;
}

void Web::handle_command() {
  if (serial->available() > 0 && bufferPos < BUFFER_SIZE) {
      // Read one byte to buffer
      buffer[bufferPos] = serial->read();

      serial->print("Reading: ");
      serial->println(buffer[bufferPos]);
      
      // Add length to buffer
      bufferPos++;
    } else if (bufferPos > 0) {
      if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '2') {
        // cmd:2, toggle auto
        cmd_auto = !cmd_auto;

        serial->print("Toggled auto to ");
        serial->println(cmd_auto);

        bufferPos = 0;
      }

      if (buffer[0] == 'c' && buffer[1] == ':' && buffer[2] == '3') {
        // cmd:2, toggle auto
        cmd_brushes = !cmd_brushes;

        serial->print("Toggled cmd_brushes to ");
        serial->println(cmd_brushes);
        
        bufferPos = 0;
      }
    }
}