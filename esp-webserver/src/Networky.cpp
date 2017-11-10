#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../include/Networky.h" // This is because arduino does not know where heders are and we cane include the folder in the build instructions

#define STA_MODE "STA"
#define AP_MODE "AP"

bool createAPConnection(const char *ssid, const char *password)
{
    Serial.println("Setting soft-AP...");
    boolean result;
    
    if (password != NULL && (password[0]=='\0')){ // Empty password passed in
        result = WiFi.softAP(ssid);
    }else{
        result = WiFi.softAP(ssid,password);
    }
    
    if (result == true){
      Serial.println("Connection is ready...");
    }else{
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

bool Networky::connectSta(const char *staSsid, const char *staPassword)
{
    this->staSsid = staSsid;
    this->staPassword = staPassword;
    this->networkState = STA_MODE;
}

void Networky::printConfigToSerial()
{
    Serial.println("__Current Network Config__");
    Serial.print("Network mode: ");
    Serial.println(this->networkState);
    
    const char *ssid = "";
    const char *password = "";

    if (this->networkState == STA_MODE){
        ssid = this->staSsid;
        password = this->staPassword;
    }else{
        ssid = this->apSsid;
        password = this->apPassword;
    }
    
    Serial.print("Ssid: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
}