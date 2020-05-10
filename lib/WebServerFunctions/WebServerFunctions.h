#ifndef WEBSERVERFUNCTIONS_H_
#define WEBSERVERFUNCTIONS_H_

#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "Constants.h"

extern ESP8266WebServer WebServer;
extern WebSocketsServer WebSocketServer;

void startWebServer();

#endif
