#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SoftwareSerial.h>

#include "Secrets.h"

#define ARDUINO_RX D4
#define ARDUINO_TX D2
#define BAUD_RATE 19200

#define IRID 0b00010000

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);
EspSoftwareSerial::UART arduinoSerial;

const int led = 13;

char serialBuffer[1024] = {1};
int bufferPos = 0;

void handleRoot()
{
  String text = "<html><pre>";

  // Search for stop start indicator
  for (int i=0; i<1022; i++) {
    if(serialBuffer[i] == 128 && serialBuffer[i+1] == 0){
      i+=2;

      uint16_t value1 = serialBuffer[i+1];
      uint8_t value2 = serialBuffer[i+2];

      text += "id:";
      text += String(int(serialBuffer[i]));
      text += "val:";
      text += String(value1 << 8 | value2);
      text += "\n";
    }
  }

  text += "</pre><form action=\"auto\"><input type=\"submit\" value=\"auto toggle\">Button</input></form></html>";

  server.send(200, "text/html", text);
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

  server.on("/auto", []() {
    arduinoSerial.print("c:3");
    server.send(200, "text/plain", "this works as well"); });

  server.begin();
}

void loop(void)
{
  server.handleClient();

  if (arduinoSerial.available() > 0)
  {
    if (bufferPos >= 1024)
    {
      bufferPos = 768;

      for (int i = 256; i < 1024; i++)
      {
        serialBuffer[i - 256] = serialBuffer[i];
      }

      for (int i = 768; i < 1024; i++)
      {
        serialBuffer[i] = 0;
      }
    }

    // Write new Serial value
    int val = arduinoSerial.read();
    serialBuffer[bufferPos] = val;
    // Serial.write(val);
    bufferPos++;
  }

  if (Serial.available() > 0)
  {
    arduinoSerial.write(Serial.read());
  }
}
