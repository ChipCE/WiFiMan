#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct Config
{
    char wifiSsid[33];
    char wifiPasswd[33];
    char mqttAddr[129];
    int mqttPort;
    char mqttUsername[33];
    char mqttPasswd[33];
    char mqttSub[129];
    char mqttPub[129];
    char mqttId[33];
    char masterPasswd[33];
    IPAddress localIP;
    char mdnsName[33];
};
#endif
