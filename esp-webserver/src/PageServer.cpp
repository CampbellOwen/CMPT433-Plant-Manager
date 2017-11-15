// #include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "../include/PageServer.h"

#define BUFFER_SIZE 1024

boolean PageServer::readHtmlFile(const char *fileName, char *buffer)
{
  // Set up SPIFFS: http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  SPIFFS.begin();
  file = SPIFFS.open(fileName, "r");
  
  if (!file){
    Serial.println("File failed to open");
    return false;
  }
  
  while(file.available()){
    file.readBytes(buffer, BUFFER_SIZE);
  }

  file.close();
}

String prepareHtmlPage(const char* file)
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "\r\n" 
            +
            file
            +
            "\r\n";
  return htmlPage;
}

void PageServer::serveBaseRootPath(){
 
}

PageServer::PageServer(uint16_t port, const char *file_path)
{
  char buffer[BUFFER_SIZE];
  server = new ESP8266WebServer(8080);
  server->on("/", HTTP_GET, std::bind(&PageServer::serveBaseRootPath, this));
  
  boolean result = readHtmlFile(file_path, buffer);
  
  if(result){
    HTML_DOC = prepareHtmlPage((const char*)buffer);
    server->begin();
    Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
  }else{
    Serial.println("Server initialization failed");
  }
}

void PageServer::servePage()
{
  server->handleClient();
}