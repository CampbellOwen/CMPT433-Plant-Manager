#include "./include/Networky.h"
#include "./include/PageServer.h"

#define AP_SSID "Plant_1"
#define AP_PASSWORD ""
#define FILE_PATH "/test.html"

Networky *network;
PageServer *server;

void setup()
{
  Serial.begin(9600);
  Serial.println();
  network = new Networky(AP_SSID, AP_PASSWORD);
  server = new PageServer(8080, FILE_PATH);
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  server->servePage(); // This sends the page to every connected client
}


