/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266OTA.h>

const char* host = "esp8266-webupdate";
const char* ssid = "........";
const char* password = "........";

ESP8266WebServer httpServer(80);
ESP8266OTA otaUpdater;

void setup(void){

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);
  
  //set web UI
  otaUpdater.setUpdaterUi("Title","Banner","Build : 0.01","Branch : master","Device info : ukn","footer");

  //setup web UI , with chip ID auto generated
  //otaUpdater.setUpdaterUi("Title","Banner","Build : 0.01","Branch : master","footer");

  otaUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void){
  httpServer.handleClient();
}
