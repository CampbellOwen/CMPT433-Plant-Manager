#include <Arduino.h>
#include "../include/Networky.h"


Networky::Networky(char *apSsid, char *apPassword){
    this->apSsid = apSsid;
    this->apPassword = apPassword;
}

bool Networky::connectSta(char *staSsid, char *staPassword){
    this->staSsid = staSsid;
    this->staPassword = staPassword;
}

void Networky::printConfigToSerial(){
    Serial.println("Hello World");
}