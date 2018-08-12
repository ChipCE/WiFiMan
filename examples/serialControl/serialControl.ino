#include <WiFiMan.h>

//WiFiMan must be declare as global var if you wand to use serialControl in loop()
WiFiMan wman;
Config conf;

void setup() 
{
  //create wman object with serial control enabled
  wman = WiFiMan(false,true,false);
  
  wman.start();
  //durring softAP mode , device will wait for serial command , and execute if the command is in valid format.Read "readme.md" for more information

  if(wman.getConfig(&conf))
  {
    //display device status
    Serial.println("Connected to AP");
    Serial.print("SSID : ");
    Serial.println(conf.wifiSsid);
    Serial.print("Passwd : ");
    Serial.println(conf.wifiPasswd);
    Serial.print("MQTT server : ");
    Serial.println(conf.mqttAddr);
    Serial.print("MQTT password : ");
    Serial.println(conf.mqttPasswd);
    Serial.print("MQTT username : ");
    Serial.println(conf.mqttUsername);
    Serial.print("MQTT Id : ");
    Serial.println(conf.mqttId);
    Serial.print("Sub topic : ");
    Serial.println(conf.mqttSub);
    Serial.print("Pub topic : ");
    Serial.println(conf.mqttPub);
    Serial.print("MQTT port : ");
    Serial.println(conf.mqttPort);
    Serial.print("Master password : ");
    Serial.println(conf.masterPasswd);
    Serial.print("Device mDNS name : ");
    Serial.println(conf.mdnsName);
    Serial.print("IP : ");
    Serial.println(conf.localIP);
  }
}

void loop() {
    //device will wait for serial command , and execute if the command is in valid format.Read "readme.md" for more information
    wman.handleSerial();
}
