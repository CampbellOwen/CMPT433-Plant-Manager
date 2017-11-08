#include <ESP8266WiFi.h>
#include "FS.h"



// Global properties
File file;
char buffer[1024];
String HTML_DOC = "";
// Start WIFI server on port 80
WiFiServer server(8080);

// Public Methods 
String prepareHtmlPage(String file)
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

void setup()
{
  // Set up SPIFFS: http://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  SPIFFS.begin();
  Serial.begin(9600);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("Can't be beet plant 1");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  
  file = SPIFFS.open("/test.html", "r");
  if (!file) {
      Serial.println("file open failed");
  }
  while (file.available()) {
    file.readBytes(buffer, 1024);
  }
  // Serial.printf(buffer);
  HTML_DOC = prepareHtmlPage(String(buffer));
  file.close();

  // Start the web server
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}

void loop()
{
  // Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(HTML_DOC);
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
  delay(200);
}


