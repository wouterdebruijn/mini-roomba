#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "Secrets.h"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

char serialBuffer[1024];
int bufferPos = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", serialBuffer);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
}

void loop(void) {
  server.handleClient();

  
  if (Serial.available() > 0) {
    if (bufferPos >= 1008) {
      // Shift buffer by 16
      for (int i=16; i < 1024; i++) {
        serialBuffer[i-16] = serialBuffer[i];
      }
      bufferPos = 1007;
    }

    // Write new Serial value
    serialBuffer[bufferPos] = Serial.read();

  }
}
