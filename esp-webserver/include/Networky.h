#ifndef NETWORKY_H
#define NETWORKY_H

class Networky{
protected:
    const char *staSsid;
    const char *staPassword;
    const char *apSsid;
    const char *apPassword;
    const char *networkState;
public:
    Networky(const char *apSsid, const char *apPassword); // Constructor to begin he node in access point mode
    bool connectSta(const char *staSsid, const char *staPassword);
    void printConfigToSerial();
};

#endif