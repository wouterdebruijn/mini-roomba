#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SoftwareSerial.h>

#include "Secrets.h"
#include "Sensor.h"
#include "Eyes.h"
#include "Buzzer.h"

#include "Index.h"

#define ARDUINO_RX D4
#define ARDUINO_TX D2
#define BAUD_RATE 19200

#ifndef APSSID
#define APSSID "Roombie"
#define APPSK "roombie12345"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
EspSoftwareSerial::UART arduinoSerial;

Servo eyesServo;

Eyes eyes(&eyesServo, D5, D6, D7, 0);

Buzzer buzzer(D8, 0);

Sensor sensors[4] = {
    Sensor(0), // FrontLeft
    Sensor(1), // FrontRight
    Sensor(2), // BackLeft
    Sensor(3)  // BackRight
};

void handleRoot()
{
  buzzer.happy();
  eyes.blink(2);
  server.send(200, "text/html", ROOT_HTML);
}

void setup(void)
{
  eyesServo.attach(D1);
  eyesServo.write(95);

  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  WiFi.softAP(ssid, password);

  arduinoSerial.begin(BAUD_RATE, EspSoftwareSerial::SWSERIAL_8N1, ARDUINO_RX, ARDUINO_TX);

  // Wait for AP to be ready
  while (WiFi.softAPgetStationNum() == 0)
  {
    delay(100);
  }
  server.on("/", handleRoot);

  server.on("/api/command", HTTP_POST, []()
            {
    if (server.hasArg("plain")) {
      String command = server.arg("plain");
      arduinoSerial.print(command);
    }

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Command received"); });

  server.on("/api/sensors", HTTP_GET, []()
            {
    String json = "[";
    for (int i = 0; i < 4; i++)
    {
      json += sensors[i].asJson();
      if (i < 3)
      {
        json += ",";
      }
    }
    json += "]";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json); });

  server.begin();
}

#define BUFFER_SIZE 16
char serialBuffer[BUFFER_SIZE] = {1};
int bufferPos = 0;

void cleanBuffer()
{
  for (uint8_t i = 0; i < BUFFER_SIZE; i++)
  {
    serialBuffer[i] = 0;
  }
  bufferPos = 0;
}

void handlebuffer()
{
  // Search for stop start indicator
  for (int i = 0; i < BUFFER_SIZE - 2; i++)
  {
    if (serialBuffer[i] == 0 && serialBuffer[i + 4] == 127, serialBuffer[i + 5] == 129)
    {
      // Amount of start bits to skip
      i += 1;

      uint16_t value1 = serialBuffer[i + 1];
      uint8_t value2 = serialBuffer[i + 2];

      uint8_t id = uint8_t(serialBuffer[i]);

      for (uint8_t j = 0; j < 4; j++)
      {
        if (sensors[j].getId() == id)
        {
          sensors[j].setValue(value1 << 8 | value2);
          break;
        }
      }

      if (eyes.getId() == id)
      {
        // Unique value for blibk trigger
        if (value2 < 8)
        {
          eyes.blink(value2 - 1);
        }
        else
        {
          eyes.set(value2);
        }
      }

      if (buzzer.getId() == id)
      {
        switch (value2)
        {
        case 1:
          buzzer.happy();
          break;
        case 2:
          buzzer.sad();
          break;
        case 3:
          buzzer.surprise();
          break;
        case 4:
          buzzer.angry();
          break;
        }
      }
    }
  }
}

void handleSerialBuffer()
{
  // Block until buffer is read
  if (arduinoSerial.available())
  {
    int val = arduinoSerial.read();
    serialBuffer[bufferPos] = val;

    // Found 2 of our end bytes, and we are at buffer index 6+
    if (bufferPos > 4 && serialBuffer[bufferPos - 1] == 127 && serialBuffer[bufferPos] == 129)
    {
      handlebuffer();
      cleanBuffer();

      return;
    }

    // We are at the end of our buffer without finding a command, what is going on here?!
    if (bufferPos >= BUFFER_SIZE - 1)
    {
      cleanBuffer();
    }
    else
    {
      bufferPos++;
    }
  }
}

void loop(void)
{
  server.handleClient();
  handleSerialBuffer();

  if (buzzer.available() > 0)
  {
    // Handle buzzer, can delay for 100ms
    buzzer.handle();
  }
}
