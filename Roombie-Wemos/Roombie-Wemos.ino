#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Wheel.h"

#include "SECRETS.h"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

Wheel rightWheel(D5, D6, D0);
Wheel leftWheel(D7, D8, D0);

void handleRoot() {
  if (server.method() != HTTP_POST) {
    server.send(200, "text/plain", "Get Method received.");
  }

  String body = server.arg("plain");

  // Parse json for new operation mode


  // Code for testing, toggle drive.

}

void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/api", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
