#ifndef ROOMBAAPI_h
#define ROOMBAAPI_h

#include <ESP8266WebServer.h>

class RoombaApi {
  private:
    ESP8266WebServer webserver;
    void handleApi();
    void handleApiPost(String body);
    void handleApiGet();

  public:
    RoombaApi();
};

#endif // ROOMBAAPI_h