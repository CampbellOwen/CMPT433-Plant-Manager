#ifndef NETWORKY_H
#define NETWORKY_H


class Networky{

protected:
    char *staSsid;
    char *staPassword;
    char *apSsid;
    char *apPassword;
public:
    Networky(char *apSsid, char *apPassword); // Constructor to begin he node in access point mode
    bool connectSta(char *staSsid, char *staPassword);
    void printConfigToSerial();
};

#endif