#include "WebServerFunctions.h"

ESP8266WebServer WebServer(80);
WebSocketsServer WebSocketServer = WebSocketsServer(81);

void handleNotFound();
bool handleFileRead(String path);
String getContentType(String filename);
void startWebSocket();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void saveWiFiConfig(uint8_t num, JsonDocument doc);

void startWebServer() 
{
  startWebSocket();

  // Start a HTTP server with a file read handler and an upload handler
  WebServer.onNotFound(handleNotFound); // if someone requests any other file or page, go to function 'handleNotFound' and check if the file exists

  WebServer.begin();                             // start the HTTP server
  Serial.println("HTTP server started.");
}

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(WebServer.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    WebServer.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "wifiConfiguration.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) 
  { 
    // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = WebServer.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void startWebSocket()
{
    WebSocketServer.begin();
    WebSocketServer.onEvent(webSocketEvent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    for (unsigned int i = 0; i < length; i++)
    {
      char c = (char)payload[i];
      Serial.print(c);
    }
    Serial.println();

    StaticJsonDocument<500> doc;
   
    DeserializationError err = deserializeJson(doc, payload);
    if (err == DeserializationError::Ok) 
    {

      Serial.print("SSID FROM FROM: ");
      const char *ssi = doc["ssid"];
      Serial.println(ssi);
      saveWiFiConfig(num, doc);
      WebSocketServer.sendTXT(num, "Saved.");

      File file = SPIFFS.open(WiFiConfigPath, "r");

      WebSocketServer.sendTXT(num, file.readString().c_str());
    }
  }
}

void saveWiFiConfig(uint8_t num, JsonDocument doc)
{
  SPIFFS.remove(WiFiConfigPath);

  File file = SPIFFS.open(WiFiConfigPath, "w");
  if (!file)
  {
    Serial.println("Failed to create the config file.");
    return;
  }

  if(serializeJson(doc, file) == 0)
  {
    Serial.println("Failed to write to file.");
  }
  else 
  {
    WebSocketServer.sendTXT(num, "Configuration Saved.");
  }

  file.close();
}