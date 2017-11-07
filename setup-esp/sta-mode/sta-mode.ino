// CMPT 433: Project
// Sources: https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/server-examples.rst

#include <ESP8266WiFi.h>

bool ssidSet = false;
bool passwordSet = false;
String ssid;
String password;

WiFiServer server(80);

void connectWiFi() {
  if ((ssidSet) && (passwordSet)) {
    Serial.printf("Connecting to %s ", ssid.c_str());
    unsigned long startTime = millis();
    unsigned long endTime = millis();
    WiFi.begin(ssid.c_str(), password.c_str());

    // Exit the loop if it has tried connecting for more than 15s
    while (((endTime - startTime) <= 15000) && (WiFi.status() != WL_CONNECT_FAILED)
      && (WiFi.status() != WL_CONNECTED) && (WiFi.status() != WL_NO_SSID_AVAIL))
    {
      delay(500);
      Serial.print(".");
      endTime = millis();
    }

    if ((endTime - startTime) > 15000)
    {
      Serial.printf("Sorry. The ESP8266 was not able to connect to %s within the timeframe.\n", ssid.c_str());
    }
    else if (WiFi.status() == WL_CONNECT_FAILED)
    {
      Serial.printf("Failed to connect to network.\n");
    }
    else if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println(" connected");
      server.begin();
      Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
    }
    else if (WiFi.status() == WL_NO_SSID_AVAIL)
    {
      Serial.printf("There is no network with the SSID %s", ssid.c_str());
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  connectWiFi();
}

void updateSSID(String newSSID)
{
  ssid = newSSID;
  Serial.printf("Updated SSID to %s\n", ssid.c_str());
  ssidSet = true;
  connectWiFi();
}

void updatePassword(String newPassword)
{
  password = newPassword;
  Serial.printf("Updated password to %s\n", password.c_str());
  passwordSet = true;
  connectWiFi();
}

void readInSerial()
{
  if (Serial.available() > 0)
  {
    String readString = Serial.readStringUntil('\n');
    if (readString.startsWith("ssid:"))
    {
      updateSSID(readString.substring(5));
    }
    if (readString.startsWith("password:"))
    {
      updatePassword(readString.substring(9));
    }
  }
}

// prepare a web page to be send to a client (web browser)
String prepareHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Analog input:  " + String(analogRead(A0)) +
            "</html>" +
            "\r\n";
  return htmlPage;
}

void loop()
{
  readInSerial();
  
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
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");
  }
}
