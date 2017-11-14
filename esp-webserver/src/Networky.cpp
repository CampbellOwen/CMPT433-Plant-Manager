#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../include/Networky.h" // This is because arduino does not know where heders are and we cane include the folder in the build instructions

#define STA_MODE "STA"
#define AP_MODE "AP"

bool createAPConnection(const char *ssid, const char *password)
{
    Serial.println("Setting soft-AP...");
    boolean result;

    if (password != NULL && (password[0] == '\0'))
    { // Empty password passed in
        result = WiFi.softAP(ssid);
    }
    else
    {
        result = WiFi.softAP(ssid, password);
    }

    if (result == true)
    {
        Serial.println("Connection is ready...");
    }
    else
    {
        Serial.println("Connection failed...");
    }
}

Networky::Networky(const char *apSsid, const char *apPassword)
{
    this->apSsid = apSsid;
    this->apPassword = apPassword;
    this->networkState = AP_MODE;
    createAPConnection(apSsid, apPassword);
}

bool Networky::establishApNetwork(const char *apSsid, const char *apPassword)
{
    this->apSsid = staSsid;
    this->apPassword = staPassword;
    this->networkState = AP_MODE;
    createAPConnection(this->apSsid, this->apPassword);
}

bool Networky::connectToSta(const char *apSsid, const char *apPassword)
{
    this->apSsid = apSsid;
    this->apPassword = apPassword;
    this->networkState = STA_MODE;
    Serial.printf("Connecting to %s ", this->apSsid);
    unsigned long startTime = millis();
    unsigned long endTime = millis();
    WiFi.begin(this->apSsid, this->apPassword);

    // Exit the loop if it has tried connecting for more than 15s
    while (((endTime - startTime) <= 15000) && (WiFi.status() != WL_CONNECT_FAILED) && (WiFi.status() != WL_CONNECTED) && (WiFi.status() != WL_NO_SSID_AVAIL))
    {
        delay(500);
        Serial.print(".");
        endTime = millis();
    }

    if ((endTime - startTime) > 15000)
    {
        Serial.printf("Sorry. The ESP8266 was not able to connect to %s within the timeframe.\n", this->apSsid);
    }
    else if (WiFi.status() == WL_CONNECT_FAILED)
    {
        Serial.printf("Failed to connect to network.\n");
    }
    else if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(" connected");
        Serial.printf("Local-ip: %s", WiFi.localIP().toString().c_str());
    }
    else if (WiFi.status() == WL_NO_SSID_AVAIL)
    {
        Serial.printf("There is no network with the SSID %s", this->apSsid);
    }
}

void Networky::connectToSta()
{
    this->connectToSta(this->apSsid, this->apPassword);
}

void Networky::setStaSsid(const char *ssid)
{
    this->apSsid = ssid;
    Serial.printf("Updated SSID to %s\n", this->apSsid);
}

void Networky::setStaPassword(const char *password)
{
    this->apPassword = password;
    Serial.printf("Updated password to %s\n", this->apPassword);
}

void Networky::printConfigToSerial()
{
    Serial.println("__Current Network Config__");
    Serial.print("Network mode: ");
    Serial.println(this->networkState);

    const char *ssid = "";
    const char *password = "";

    if (this->networkState == STA_MODE)
    {
        ssid = this->staSsid;
        password = this->staPassword;
    }
    else
    {
        ssid = this->apSsid;
        password = this->apPassword;
    }

    Serial.print("Ssid: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.printf("Local_ip: %s", WiFi.localIP().toString().c_str());
}