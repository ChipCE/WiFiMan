#include <WiFiMan.h>

Config conf;

void setup() 
{
  //create default object
  WiFiMan wman = WiFiMan();
  
  //force into config mode if pin 13 is set to LOW
  Serial.begin(115200);
  Serial.println("Set pin 13 to LOW within 10 seconds to skip auto connect.");
  pinMode(13,INPUT_PULLUP);
  long startW = millis();
  while((millis()-startW)<10*1000)
  {
    if(digitalRead(13)==LOW)
    {
      wman.forceApMode();
      Serial.println("Auto-connect will be skip.");
      break;
    }
  }

  //start WiFiMan
  wman.start();

  if(wman.getConfig(&conf))
  {

  }
}

void loop() {
}
