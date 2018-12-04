#include <WiFiMan.h>
Config conf;
CustomConfig customConf;
void setup() 
{
  Serial.begin(115200);
  
  //create default object
  WiFiMan wman = WiFiMan();

  //add custom config, this must be called before wman.start()
  wman.addCustomArg("test-number","6","number","test number");
  wman.addCustomArg("test-password","6","password","test password");
  wman.addCustomArg("test-txt","6","text","test txt");
  
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

  //get custom config parameters
  if(wman.getCustomConfig(&customConf))
  {
    for(int i=0;i<customConf.count;i++)
    {
      Serial.print(customConf.args[i].key);
      Serial.print(" = ");
      Serial.println(customConf.args[i].value);
    }
  }
}

void loop() {
  //
}