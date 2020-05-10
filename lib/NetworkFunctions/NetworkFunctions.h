#ifndef NETWORKFUNCTIONS_H_
#define NETWORKFUNCTIONS_H_

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "WifiConfigStruct.h"
#include "Constants.h"

extern bool NETWORK_AS_AP;
extern bool NETWORK_AS_STATION;

void startNetwork();

#endif
