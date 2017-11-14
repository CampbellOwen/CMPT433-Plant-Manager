#ifndef NETWORKY_H
#define NETWORKY_H

class Networky{
protected:
    const char *staSsid;
    const char *staPassword;
    const char *apSsid;
    const char *apPassword;
    const char *networkState;
    void connectToSta();
public:
    Networky(const char *apSsid, const char *apPassword); // Constructor to begin he node in access point mode
    bool connectToSta(const char *staSsid, const char *staPassword);
    bool establishApNetwork(const char *staSsid, const char *staPassword);
    void setStaSsid(const char *ssid);
    void setStaPassword(const char *password);
    void printConfigToSerial();
};

#endif