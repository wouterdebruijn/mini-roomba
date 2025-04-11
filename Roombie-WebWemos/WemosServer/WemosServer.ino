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

Eyes eyes(D5, D6, D7);

Sensor sensors[] = {
    Sensor(0x00), // FrontLeft
    Sensor(0x01), // FrontRight
    Sensor(0x02), // BackLeft
    Sensor(0x03)  // BackRight
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

  server.on("/happiness", HTTP_POST, []()
            {
    if (server.hasArg("happiness"))
    {
      // Get the happiness value from the POST request
      uint8_t happiness = server.arg("happiness").toInt();

      // Call the set method to update the LED color
      eyes.set(happiness);

      // Respond with a success message
      server.send(200, "text/plain", "Happiness value received and LEDs updated.");
    }
    else
    {
      // If the happiness parameter is not in the request
      server.send(400, "text/plain", "Missing happiness value.");
    } });

  server.begin();
}

#define BUFFER_SIZE 128
char serialBuffer[BUFFER_SIZE] = {1};
int bufferPos = 0;

void handlebuffer() {
  Serial.println("handle");
  // Search for stop start indicator
  for (int i = 0; i < BUFFER_SIZE-2; i++)
  {
    if (serialBuffer[i] == 128 && serialBuffer[i + 1] == 0)
    {
      i += 2;

      uint16_t value1 = serialBuffer[i + 1];
      uint8_t value2 = serialBuffer[i + 2];

      for (uint8_t j = 0; j < sizeof(sensors); j++)
      {
        if (sensors[j].getId() == int(serialBuffer[i]))
        {
          sensors[j].setValue(value1 << 8 | value2);
          Serial.println("Sensor val set.");
          break;
        }
      }
    }
  }
}

void loop(void)
{
  server.handleClient();

  // Block until buffer is read
  if (arduinoSerial.available() > 0)
  {
    int val = arduinoSerial.read();
    serialBuffer[bufferPos] = val;
    bufferPos++;
  }

  if (bufferPos > 16) {
    handlebuffer();

    for (int i=0; i<BUFFER_SIZE; i++) {
      serialBuffer[i] = 0;
    }
    bufferPos = 0;
  }
}
