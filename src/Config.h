#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct Config
{
    char *wifiSsid;
    char *wifiPasswd;
    char *mqttAddr;
    int mqttPort;
    char *mqttUsername;
    char *mqttPasswd;
    char *mqttSub;
    char *mqttPub;
    char *mqttId;
    char *masterPasswd;

    IPAddress localIP;
    char *mdnsName;
};
#endif
