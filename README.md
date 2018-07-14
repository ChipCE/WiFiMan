# WiFiMan
Wifi manager for ESP8266 with configurable web UI and ability to config mqtt, OTA update,config via serial   
Require library <a href="https://github.com/ChipTechno/ESP8266OTA">ESP8266OTA</a>

## About password setting
- Soft AP password : Password of ESP8266 when in AP mode .Can be set with setApPasswd("yourPassword").If default password is not set, AP will fireup without password.
- HTTP access password : Password use to access HTTP config page.Default password and username can be set with setHttpPassword("youPassword") and setHttpUsername("yourUsername").If default password and username are not set, HTTP config page can be access without authentication.After masterPassword is set, HTTP access will use masterPassword instead of defaultPassword
- Master password : Password to save/edit device config. The default master password is blank, you must change it after first login.


## To-do
