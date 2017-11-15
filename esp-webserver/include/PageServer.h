#ifndef PAGESERVER_H
#define PAGESERVER_H
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

class PageServer{
protected:
  File file;
  ESP8266WebServer *server;
  String HTML_DOC;
  WiFiClient client;
  void serveBaseRootPath();
  boolean readHtmlFile(const char *fileName, char *buffer);
public:
  PageServer(uint16_t port, const char *file_path);
  void servePage(); // Servers the page passed in from the constructor

};

#endif