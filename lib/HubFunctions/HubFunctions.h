#ifndef HUBFUNCTIONS_H_
#define HUNFUNCTIONS_H_

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "HubConfigStruct.h"
#include "Constants.h"

void startHub();
void ringBell();
void websocketClientLoop();
#endif
