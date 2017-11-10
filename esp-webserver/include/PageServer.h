#ifndef PAGESERVER_H
#define PAGESERVER_H
#include <ESP8266WiFi.h>
#include <FS.h>

class PageServer{
protected:
  File file;
  WiFiServer *server;
  String HTML_DOC;
  WiFiClient client;
  boolean readHtmlFile(const char *fileName, char *buffer);
public:
  PageServer(uint16_t port, const char *file_path);
  void servePage(); // Servers the page passed in from the constructor

};

#endif