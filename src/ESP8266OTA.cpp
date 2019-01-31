#include "ESP8266OTA.h"


ESP8266OTA::ESP8266OTA()
{
  _server = NULL;
  _authenticated = false;
  #ifdef DEBUG_ESP_PORT
    DEBUG_ESP_PORT.begin(115200);
    DEBUG_ESP_PORT.setDebugOutput(false);
  #endif
}

void ESP8266OTA::setup(ESP8266WebServer *server)
{
  setup(server, "", "");
}

void ESP8266OTA::setup(ESP8266WebServer *server, const char * path)
{
  setup(server, path, "", "");
}

void ESP8266OTA::setup(ESP8266WebServer *server, String username, String password)
{
  setup(server, "/update", username, password);
}


void ESP8266OTA::setup(ESP8266WebServer *server, const char * path, String username, String password)
{
    _server = server;
    _username = username;
    _password = password;

    // handler for the /update form page
    _server->on(path, HTTP_GET, [&](){
      if(_username != "" && _password != "" && !_server->authenticate(_username.c_str(), _password.c_str()))
        return _server->requestAuthentication();

      //get page

      String pageIndex = FPSTR(HTTP_HEADER);
      pageIndex.concat(FPSTR(HTTP_UPDATER_INDEX));
      pageIndex.concat(FPSTR(HTTP_FOOTER));

      pageIndex.replace("{title}",_title);
      pageIndex.replace("{banner}",_banner);
      pageIndex.replace("{build}",_build);
      pageIndex.replace("{branch}",_branch);
      pageIndex.replace("{deviceInfo}",_deviceInfo);
      pageIndex.replace("{footer}",_footer);

      applyTheme(pageIndex);

      _server->send(200, "text/html", pageIndex);
    });

    // handler for the /update form POST (once file upload finishes)
    _server->on(path, HTTP_POST, [&](){
      if(!_authenticated)
        return _server->requestAuthentication();

      if(Update.hasError())
      {
        String pageFailed = FPSTR(HTTP_HEADERRELOAD);
        pageFailed.concat(FPSTR(HTTP_UPDATER_FAILED));
        pageFailed.concat(FPSTR(HTTP_FOOTER));

        pageFailed.replace("{title}",_title);
        pageFailed.replace("{banner}",_banner);
        pageFailed.replace("{build}",_build);
        pageFailed.replace("{branch}",_branch);
        pageFailed.replace("{deviceInfo}",_deviceInfo);
        pageFailed.replace("{footer}",_footer);
        pageFailed.replace("{url}","/");
        pageFailed.replace("{delay}","30");

        applyTheme(pageFailed);

        _server->send(200, "text/html",pageFailed);
        delay(3000);
        rebootToApMode();
      }
      else
      {
        String pageSuccess = FPSTR(HTTP_HEADERRELOAD);
        pageSuccess.concat(FPSTR(HTTP_UPDATER_SUCCESS));
        pageSuccess.concat(FPSTR(HTTP_FOOTER));

        pageSuccess.replace("{title}",_title);
        pageSuccess.replace("{banner}",_banner);
        pageSuccess.replace("{build}",_build);
        pageSuccess.replace("{branch}",_branch);
        pageSuccess.replace("{deviceInfo}",_deviceInfo);
        pageSuccess.replace("{footer}",_footer);
        pageSuccess.replace("{url}","/");
        pageSuccess.replace("{delay}","30");

        applyTheme(pageSuccess);

        _server->send(200, "text/html",pageSuccess);
        delay(3000);
        reboot();
      }
    },[&](){
      // handler for the file upload, get's the sketch bytes, and writes
      // them through the Update object
      HTTPUpload& upload = _server->upload();
      if(upload.status == UPLOAD_FILE_START){

        _authenticated = (_username == "" || _password == "" || _server->authenticate(_username.c_str(), _password.c_str()));
        if(!_authenticated){
          DEBUG_MSG("#__ Unauthenticated Update\n");
          return;
        }

        WiFiUDP::stopAll();
        DEBUG_MSG("#__ Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          #ifdef DEBUG_ESP_PORT 
            Update.printError(DEBUG_ESP_PORT); 
          #endif
        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_WRITE){
        DEBUG_MSG("#__ .\n");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          #ifdef DEBUG_ESP_PORT 
          Update.printError(Serial);
          #endif

        }
      } else if(_authenticated && upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          DEBUG_MSG("#__ Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          #ifdef DEBUG_ESP_PORT 
            Update.printError(DEBUG_ESP_PORT);
          #endif
        }
        #ifdef DEBUG_ESP_PORT 
          DEBUG_ESP_PORT.setDebugOutput(false);
        #endif
      } else if(_authenticated && upload.status == UPLOAD_FILE_ABORTED){
        Update.end();
        DEBUG_MSG("#__ Update was aborted\n");
      }
      delay(0);
    });
}

void ESP8266OTA::setTitle(String title)
{
  _title = title;
}

void ESP8266OTA::setBanner(String banner)
{
  _banner = banner;
}

void ESP8266OTA::setBuild(String build)
{
  _build = build;
}

void ESP8266OTA::setBranch(String branch)
{
  _branch = branch;
}

void ESP8266OTA::setDeviceInfo(String deviceInfo)
{
  _deviceInfo = deviceInfo;
}

void ESP8266OTA::setFooter(String footer)
{
  _footer = footer;
}

void ESP8266OTA::setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer)
{
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = deviceInfo;
    _footer = footer;

}

void ESP8266OTA::setUpdaterUi(String title,String banner,String build,String branch,String footer)
{
    _title = title;
    _banner = banner;
    _build = build;
    _branch = branch;
    _deviceInfo = "ChipId : " + String(ESP.getChipId());
    _footer = footer;
}

void ESP8266OTA::updatePassword(String password)
{
  _password = password;
}

void ESP8266OTA::applyTheme(String &page)
{
  page.replace("{custom-css}",FPSTR(custom_css));
}