#include "HubFunctions.h"

WebSocketsClient webSocketClient;
HubConfig hubConfig;

void webSocketClientEvent(WStype_t type, uint8_t *payload, size_t length);
void loadHubConfiguration();
void websocketClientLoop();
void ringBell();

void startHub()
{
    loadHubConfiguration();
    webSocketClient.begin(hubConfig.Host, hubConfig.Port, hubConfig.Hub);
    webSocketClient.onEvent(webSocketClientEvent);
}

void loadHubConfiguration()
{
    // Open file for reading
    File file = SPIFFS.open(HubConfigPath, "r");
    if (!file)
    {
        Serial.println("Failed to read Hub Configuration file.");
        return;
    }

    StaticJsonDocument<512> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));

    // Copy values from the JsonDocument to the Config
    hubConfig.Host = doc["host"].as<String>();
    hubConfig.Hub = doc["hub"].as<String>();
    hubConfig.UseSsl = doc["useSsl"];
    hubConfig.Port = doc["port"];

    Serial.println(hubConfig.Host);

    // Close the file (Curiously, File's destructor doesn't close the file)
    file.close();
    doc.clear();

    if (hubConfig.Host == "")
    {
        Serial.println("Hub Configuration Not Found!");
        return;
    }

    Serial.printf("Hub Configuration Found for %s", hubConfig.Host.c_str());
    Serial.println();
}

void webSocketClientEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!");
        Serial.println();
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to URL: %s", payload);
        Serial.println();
        webSocketClient.sendTXT("{\"protocol\": \"json\",\"version\": 1}\x1E");

        ringBell(); // TODO: remove this. This is here for test purpose only.

        break;
    case WStype_TEXT:
        Serial.printf("[WSc] get text: %s", payload);
        Serial.println();
        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u", length);
        Serial.println();
        break;
    }
}

void websocketClientLoop()
{
    webSocketClient.loop();
}

void ringBell()
{
    webSocketClient.sendTXT("{\"arguments\":[\"Door\",\"Someone is Ringing the bell.\"],\"invocationId\":\"1\",\"target\":\"SendMessage\",\"type\":1})\x1E"); // TODO: invocation id must be incremental
}
