/*
This is a simple example of using WiFiMan with Sonoff basic
Send "on","off","toogle","reset" or "config" to subTopic via MQTT to control the SonOff
Press the button of SonOff will toogle the state of relay. Long press will reset the device and force boot into config mode

Green led behavior
Config mode:
  Off : timeout
  On  : TRying to connect
  Blink : Config mode is active

After connected to WiFi
  Off : timeout
  On  : Trying to re-connect to WiFi or MQTT
  Blink : OK  
*/

#include <WiFiMan.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Config
#define RELAY_DEFAULT_STATE 0 //the default state of relay after device boot up. 0:off , 1:on
#define STATUS_REPORT_INTERVAL 1000 //SonOff will send its current status via MQTT to pubTopic. Send interval in millisecond
#define EDGE_MODE 0 //if set to 1 , the device will send status report only when relay status changed. STATUS_REPORT_INTERVAL will be ignored
#define ENABLE_SLEEP_TIMEOUT 0 //if set to 1 , the device will go to sleep mode if cannot connect to wifi or config-timeout
#define RESET_THRESHOLD 300 //Long press duration of button to reset the device. The this too long will crash the device
#define DEBOUNCE_DELAY 500 //debound for button
#define RECONNECT_INTERVAL 2000 //time between re-connect
//End config

//Pin define
#define RELAY 12
#define G_LED 13
#define BUTTON 0
//#define R_LED 14 //with the Q4 and R23 soldered, the red led is connect to relay signal

unsigned long lastSend = 0;
unsigned long ledTimer = 0;
int lastState = -1;

Config conf;
WiFiClient espClient;
PubSubClient client(espClient);

void controlLed(bool state);
void controlRelay(bool state);
bool reconnect();
void initMqtt();
void mqttRecv(char* topic, byte* payload, unsigned int length);
void mqttSend();
void initHardware();
void handleButton();
void handleLed();
void nonBlockingDelay(unsigned long delayMs);

void setup() 
{
    //init serial
    Serial.begin(115200);

    pinMode(RELAY,OUTPUT);
    digitalWrite(RELAY,RELAY_DEFAULT_STATE);
    pinMode(BUTTON,INPUT_PULLUP);

    //wifi manager
    WiFiMan wman = WiFiMan();
    wman.setIndicatorLedPin(G_LED,LOW);
    wman.setExtFunc(handleButton);
    wman.setWebUi("SonOff config","Config portal","build : 0.01","Branch : master","SonOff Basic");
    wman.setApName("SonOff-Conf");
    wman.start();

    //init mqtt and hardware if connected success
    if(wman.getConfig(&conf))
    {
      pinMode(G_LED,OUTPUT);
      digitalWrite(G_LED,HIGH); // or just call handleLed
      DEBUG_MSG("#SonOff : Connected :3\n");
      initMqtt();
    }
    else
    {
      digitalWrite(G_LED,LOW);
      if(ENABLE_SLEEP_TIMEOUT)
      {
        DEBUG_MSG("Cannot connect to AP! Enter sleep mode ..zZ");
        ESP.deepSleep(0);
      }
    }
    Serial.println("EOS");
}

void loop() 
{
  //re-connect to wifi and mqtt server if needed
  if(reconnect())
  {
    //send status report
    mqttSend();
    //mqtt
    client.loop();
    //button
    handleButton();
    //indicator led
    handleLed();
  }
}

//reconnect to mqtt server
bool reconnect() 
{
  if (WiFi.status() != WL_CONNECTED)
  {
    for(int i=0;i<10 && (WiFi.status() != WL_CONNECTED);i++)
    {
      WiFi.begin(conf.wifiSsid, conf.wifiPasswd);
      nonBlockingDelay(RECONNECT_INTERVAL);
    }
  }
  if(WiFi.status() != WL_CONNECTED)
    return false;

  while (!client.connected()) 
  {
    DEBUG_MSG("Attempting MQTT connection...");
    if (client.connect(conf.mqttId)) 
    {
      DEBUG_MSG("connected");
      client.subscribe(conf.mqttSub);
    } 
    else 
    {
      DEBUG_MSG("failed, rc=");
      Serial.print(client.state());
      DEBUG_MSG(" try again in 5 seconds");
      nonBlockingDelay(RECONNECT_INTERVAL);
    }
  }
  if(!client.connected())
    return false;
  return true;
}

void initMqtt()
{
    Serial.println(conf.mqttAddr);
    client.setServer(conf.mqttAddr, conf.mqttPort);
    client.setCallback(mqttRecv);
}

void mqttRecv(char* topic, byte* payload, unsigned int length) 
{
  if(strncmp((char*)payload,"reset",length) == 0)
    reboot();

  if(strncmp((char*)payload,"config",length) == 0)
    rebootToApMode();

  if(strncmp((char*)payload,"on",length) == 0)
		if(!digitalRead(RELAY))
      digitalWrite(RELAY,HIGH);
	
  if(strncmp((char*)payload,"off",length) == 0)
		if(digitalRead(RELAY))
      digitalWrite(RELAY,LOW);

  if(strncmp((char*)payload,"toggle",length) == 0)
    digitalWrite(RELAY,!digitalRead(RELAY));

  /* DEFINE YOUR CUSTOM MQTT COMMANDS HERE */
}

void mqttSend()
{
  if(EDGE_MODE && (lastState != digitalRead(RELAY)))
    lastState = digitalRead(RELAY);
  else
    if( millis() - lastSend > STATUS_REPORT_INTERVAL)
      lastSend = millis();
    else
      return;
  
  if(digitalRead(RELAY))
    client.publish(conf.mqttPub, "1");
  else
    client.publish(conf.mqttPub, "0"); 
}


void handleButton()
{
  if(!digitalRead(BUTTON))
  {
    unsigned long startTime = millis();
    while(!digitalRead(BUTTON))
      if(millis()-startTime > RESET_THRESHOLD)
        rebootToApMode();
    digitalWrite(RELAY,!digitalRead(RELAY));
    delay(DEBOUNCE_DELAY);
  }
}


void nonBlockingDelay(unsigned long delayMs)
{
  unsigned long endTime = millis() + delayMs;
  while(millis() < endTime)
  {
    handleButton();
    handleLed();
  }
}

void handleLed()
{
  if((WiFi.status() == WL_CONNECTED) && client.connected())
  {
    //blink
    if(millis() - ledTimer > 1000)
    {
      ledTimer = millis();
      digitalWrite(G_LED,!digitalRead(G_LED));
    }
  }
  else
  {
    if(!digitalRead(G_LED))
      digitalWrite(G_LED,HIGH);
  }
}
