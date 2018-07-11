#include <WiFiMan.h>

void setup() 
{
  //create default object
  WiFiMan wman = WiFiMan();
  
  wman.start();

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
  //
}
