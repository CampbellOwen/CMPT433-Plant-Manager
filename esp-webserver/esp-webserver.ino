#include "./include/Networky.h"
#include "./include/PageServer.h"

#define AP_SSID "Plant_1"
#define AP_PASSWORD ""
#define STA_SSID "DTO_Guest"
#define STA_PASSWROD "#Lovemusic"
#define FILE_PATH "/test.html"

Networky *network;
PageServer *server;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  network = new Networky(AP_SSID, AP_PASSWORD);
  server = new PageServer(8080, FILE_PATH);
  // network->connectToSta(STA_SSID, STA_PASSWROD); // This works, make simple page to post the request from the webpage.
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  server->servePage(); // This sends the page to every connected client
  delay(200);
}