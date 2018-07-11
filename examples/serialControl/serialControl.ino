#include <WiFiMan.h>

//WiFiMan must be declare as global var if you wand to use serialControl in loop()
WiFiMan wman;


void setup() 
{
  //create wman object with serial control enabled
  wman = WiFiMan(false,true,false);
  
  wman.start();
  //durring softAP mode , device will wait for serial command , and execute if the command is in valid format.Read "readme.md" for more information

  if(wman.isConnected())
  {
    //display device status
    Serial.println("Connected to AP");
    Serial.print("SSID : ");
    Serial.println(wman.getWifiSsid());
    Serial.print("Passwd : ");
    Serial.println(wman.getWifiPasswd());
    Serial.print("IP : ");
    Serial.println(wman.getIp());
    Serial.print("MQTT server : ");
    Serial.println(wman.getMqttServerAddr());
    Serial.print("MQTT password : ");
    Serial.println(wman.getMqttServerPasswd());
    Serial.print("MQTT username : ");
    Serial.println(wman.getMqttUsername());
    Serial.print("MQTT Id : ");
    Serial.println(wman.getMqttId());
    Serial.print("Sub topic : ");
    Serial.println(wman.getMqttSub());
    Serial.print("Pub topic : ");
    Serial.println(wman.getMqttPub());
    Serial.print("MQTT port : ");
    Serial.println(wman.getMqttPort());
    Serial.print("Device DNS name : ");
    Serial.println(wman.getDnsName());
  }
}

void loop() {
    //device will wait for serial command , and execute if the command is in valid format.Read "readme.md" for more information
    wman.handleSerial();
}
