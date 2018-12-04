#include <WiFiMan.h>
Config conf;

void setup() 
{
  Serial.begin(115200);
  
  //create default object
  WiFiMan wman = WiFiMan();
  //wman.deleteConfig();

  //Force esp8266 to config mode , forceApMode(); must be called before wman.start()
  pinMode(13,INPUT_PULLUP);
  if(digitalRead(13)==LOW)
    wman.forceApMode();

  pinMode(12,INPUT_PULLUP);
  
  wman.start();

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
  //reboot esp8266 to config mode , rebootToApMode() can be called anywhere even when FiFiMan out of scoop
  if(digitalRead(12)==LOW)
    rebootToApMode();
  /// or use pin 0 as input.
  /*
  if(digitalRead(0)==LOW)
  {
    // delay is a must , or esp8266 will enter flash mode after reboot
    delay(2000);
    rebootToApMode();
  }

  */
}