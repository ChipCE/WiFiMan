#include <WiFiMan.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define INTERVAL 1000

Config conf;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastSend;

//reconnect to mqtt server
void reconnect() 
{
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(conf.mqttId)) 
    {
      Serial.println("connected");
      client.subscribe(conf.mqttSub);
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void initMqtt()
{
    Serial.println(conf.mqttAddr);
    client.setServer(conf.mqttAddr, conf.mqttPort);
    client.setCallback(callback);
    lastSend = 0;
}

void callback(char* topic, byte* payload, unsigned int length) 
{
    //reset the esp if received "reset" payload
    if(strncmp((char*)payload,"reset",length) == 0)
        rebootToApMode();
}

void sendMqtt()
{
    //just send "Hello, World" every one second
    if( millis() - lastSend > INTERVAL)
    {
        lastSend = millis();
        client.publish(conf.mqttPub, "Hello, World!");
    }
}

void setup() 
{
    //init serial
    Serial.begin(115200);

    WiFiMan wman = WiFiMan();
    wman.start();

    //init mqtt if connected success
    if(wman.getConfig(&conf))
        initMqtt();
    else
        ESP.deepSleep(0);
}

void loop() 
{
    reconnect();
    sendMqtt();
}
