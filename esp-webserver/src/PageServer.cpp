#include <ESP8266WiFi.h>
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
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" 
            +
            file
            +
            "\r\n";
  return htmlPage;
}

PageServer::PageServer(uint16_t port, const char *file_path)
{
  char buffer[BUFFER_SIZE];
  server = new WiFiServer(8080);
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
  client = server->available();
  
  if (client){
    Serial.println("\n[Client connected]");
    
    while (client.connected()){
      if (client.available()){

        // Read line by line what the client (web browser) is requesting
        String line = client.readStringUntil('\r'); 
        Serial.print(line);
        
        // Wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n'){
          client.println(HTML_DOC);
          break;
        }

      }
    }

    delay(1); // Give the web browser time to receive the data
    client.stop(); // Close the connection:
    Serial.println("[Client disonnected]");
  }
}