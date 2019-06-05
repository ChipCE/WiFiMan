#include <WiFiMan.h>
Config conf;

void setup() 
{
  Serial.begin(115200);
  
  //create default object
  WiFiMan wman = WiFiMan();

  //delete saved config will force esp8266 into config mode. This function must be called before wman.start().
  //wman.deleteConfig();

  //Force esp8266 into config mode. This function is same as deleteConfig(), but will not delete saved config. This function must be called before wman.start().
  //wman.forceApMode();

  //or you can Set auto-connect interrupt pin. Pull this pin down for more than 500ms will skip auto-connect process(only works when the device trying to connect to AP using saved config). This function must be called before wman.start(). The number of connect atemp can be set with setMaxConnectAttempt(int connectAttempt)
  //wman.setConfigPin(0,LOW);

  //set led indicator. The led will stay on when esp trying to connect to the network and blink when device in ap(config) mode.
  //wman.setLedPin(BUILTIN_LED,LOW);
  
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

  pinMode(12,INPUT_PULLUP);

  //you can re-use the indicator led pin :)
  // for example pinMode(13,INPUT); or pinMode(13,OUTPUT);
}

void loop() 
{
  //reboot esp8266 to config mode , rebootToApMode() can be called anywhere even when FiFiMan out of scoop
  if(digitalRead(12)==LOW)
    rebootToApMode();
}