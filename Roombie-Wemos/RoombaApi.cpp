#include "RoombaApi.h"
#include "ESP8266WebServer.h"

RoombaApi::RoombaApi() : webserver(80) {}

void RoombaApi::handleApi() {
  HTTPMethod method = webserver.method();

  // Handle POST request reading body
  if (method == HTTP_POST) {
    return handleApiPost(webserver.arg("plain"));
  }

  // Handle GET request
  if (method == HTTP_GET) {
    return handleApiGet();
  }

  webserver.send(404, "text/plain", "Method not implemented.");
}

void RoombaApi::handleApiPost(String body) {
  rightWheel.forwards();
  leftWheel.forwards();

  String message = "Roombie v0.0.0\n";

  if (rightWheel.enabled() && leftWheel.enabled()) {
    rightWheel.disable();
    leftWheel.disable();
    message += "Disabled drive\n";
  } else {
    rightWheel.enable();
    leftWheel.enable();
    message += "Enabled drive\n";
  }

  server.send(200, "text/plain", message);
}