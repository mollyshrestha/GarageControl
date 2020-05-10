#include "NetworkFunctions.h"

bool NETWORK_AS_AP = false;
bool NETWORK_AS_STATION = false;
WiFiConfig config;

bool loadConfiguration();

void startNetwork()
{
  bool isConfigured = loadConfiguration();

  if (!isConfigured)
  {
    WiFi.softAP(AccessPointSsid, AccessPointPassword); // Start the access point
    NETWORK_AS_AP = true;
    Serial.print("Access Point \"");
    Serial.print(AccessPointSsid);
    Serial.println("\" started");

    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP()); // Send the IP address of the ESP8266 to the computer
  }
  else 
  {
    WiFi.begin(config.ssid, config.password);
    while(WiFi.status() != WL_CONNECTED)
    {
      Serial.printf("Connecting to network %s...", config.ssid);
      Serial.println();
      delay(1000);
    }
    NETWORK_AS_STATION = true;

    Serial.print("IP Address: \t");
    Serial.println(WiFi.localIP());
  }
}

bool loadConfiguration() {
  // Open file for reading
  File file = SPIFFS.open(WiFiConfigPath, "r");

  StaticJsonDocument<512> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the Config
  strlcpy(config.ssid,                  // <- destination
          doc["ssid"],  // <- source
          sizeof(config.ssid));         // <- destination's capacity

  strlcpy(config.password,                  // <- destination
          doc["password"],  // <- source
          sizeof(config.password));         // <- destination's capacity

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();

  String ssid(config.ssid);
  if(ssid == "")
  {
    Serial.println("Configuration Not Found!");
    return false;
  }
  
  Serial.println("Configuration Found!");
  return true;
}
