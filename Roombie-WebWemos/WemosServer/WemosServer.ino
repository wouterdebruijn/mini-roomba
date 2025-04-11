#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SoftwareSerial.h>

#include "Secrets.h"
#include "Sensor.h"
#include "Eyes.h"

#define ARDUINO_RX D4
#define ARDUINO_TX D2
#define BAUD_RATE 19200

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);
EspSoftwareSerial::UART arduinoSerial;

const int led = 13;

Eyes eyes(D5, D6, D7, 0);

Sensor sensors[4] = {
    Sensor(0), // FrontLeft
    Sensor(1), // FrontRight
    Sensor(2), // BackLeft
    Sensor(3)  // BackRight
};

void handleRoot()
{
  server.send(200, "text/html", sensors[0].asJson() + sensors[1].asJson() + sensors[2].asJson() + sensors[3].asJson());
}

void setup(void)
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  arduinoSerial.begin(BAUD_RATE, EspSoftwareSerial::SWSERIAL_8N1, ARDUINO_RX, ARDUINO_TX);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/auto", []()
            {
    arduinoSerial.print("c:2");
    server.send(200, "text/plain", "this works as well"); });

  server.on("/wiper", []()
            {
    arduinoSerial.print("c:3");
    server.send(200, "text/plain", "this works as well"); });

  server.begin();
}

#define BUFFER_SIZE 16
char serialBuffer[BUFFER_SIZE] = {1};
int bufferPos = 0;

void cleanBuffer() {
  for (uint8_t i=0; i <BUFFER_SIZE; i++) {
    serialBuffer[i] = 0;
  }
  bufferPos=0;
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

      for (uint8_t j = 0; j < 4; j++)
      {
        if (sensors[j].getId() == uint8_t(serialBuffer[i]))
        {
          sensors[j].setValue(value1 << 8 | value2);
          break;
        }
      }

      if (eyes.getId() == uint8_t(serialBuffer[i]))
      {
        eyes.set(value2);
      }
    }
  }
}

void handleSerialBuffer() {
    // Block until buffer is read
  if (arduinoSerial.available())
  {
    int val = arduinoSerial.read();
    serialBuffer[bufferPos] = val;

    // Found 2 of our end bytes, and we are at buffer index 6+
    if (bufferPos > 4 && serialBuffer[bufferPos - 1] == 127 && serialBuffer[bufferPos] == 129) {
      handlebuffer();
      cleanBuffer();

      return;
    }

    // We are at the end of our buffer without finding a command, what is going on here?!
    if (bufferPos >= BUFFER_SIZE - 1) {
      cleanBuffer();
    } else {
      bufferPos++;
    }
  }
}

void loop(void)
{
  server.handleClient();
  handleSerialBuffer();
}
