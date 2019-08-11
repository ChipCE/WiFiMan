#include <WiFiMan.h>

Config conf;

#define BUTTON 14
#define LED 13

void functionToBeCalled()
{
  if(!digitalRead(BUTTON))
  {
    digitalWrite(LED,!digitalRead(LED));
    //debounce
    delay(100);
  }
}

void setup() 
{
  Serial.begin(115200);
  
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  //create default object
  WiFiMan wman = WiFiMan();

  //set a external function. In this example, toggle the LED when button is pressed. This function will be called in the loop of WiFiMan. Becareful when put delay in the function, it might affect the function of WiFiMan. The function return type must be void and without input parameter.
  wman.setExtFunc(functionToBeCalled);
  
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
  //
}
