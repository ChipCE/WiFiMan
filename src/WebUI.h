#ifndef __WEB_UI_H
#define __WEB_UI_H
#include <memory>

const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\" style=\"height:100%;\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\" /><title>{title}</title><style>{custom-css}</style><script>function goBack() { window.history.back(); }</script></head><body><div id=\"header\"><h2>{banner}</h2><p>{build}</p><p>{branch}</p><p>{deviceInfo}</p></div><div id=\"body\">";
const char HTTP_HEADERRELOAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\" style=\"height:100%;\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\" /><meta http-equiv=\"refresh\" content=\"{delay};url={url}\" /><title>{title}</title><style>{custom-css}</style><script>function goBack() { window.history.back(); }</script></head><body><div id=\"header\"><h2>{banner}</h2><p>{build}</p><p>{branch}</p><p>{deviceInfo}</p></div><div id=\"body\">";
const char HTTP_FOOTER[] PROGMEM = "</div><div id=\"footer\"><p>{footer}</p></div> </body></html>";
const char HTTP_INDEX[] PROGMEM = "<br/><form action=\"/config\" method=\"get\"><button>Configure</button></form><br/><form action=\"/update\" method=\"get\"><button>Update</button></form><br/><form action=\"/clearsetting\" method=\"post\"><button>Clear setting</button></form><br/><form action=\"/reset\" method=\"post\"><button>Reset</button></form><br/><form action=\"/help\" method=\"get\"><button>Help</button><br/><br/></form>";

const char HTTP_CONFIG_WIFI_DEVICE[] PROGMEM = "<form method='get' action='/save'>Wifi configuration<input name='wifiSsid' maxlength='32' placeholder='SSID' required><br/><input name='wifiPasswd' maxlength='32' type='password' placeholder='password'><br/>Device configuration<input name='mqttId' maxlength='32' placeholder='Device ID / MQTT ID'><br/>";
const char HTTP_CONFIG_MQTT[] PROGMEM = "MQTT configuration<input name='mqttAddr' maxlength='128' placeholder='MQTT server' required><br/><input name='mqttPort' maxlength='8' type='number' placeholder='MQTT port' required><br/><input name='mqttUsername' maxlength='32' placeholder='MQTT Username'><br/><input name='mqttPasswd' maxlength='32' type='password' placeholder='MQTT password'><br/><input name='mqttSub' maxlength='128' placeholder='Subscribe topic'><br/><input name='mqttPub' maxlength='128' placeholder='Publish topic'><br/>";
const char HTTP_CONFIG_AUTH[] PROGMEM = "Security configuration<input name='masterPasswd' maxlength='32' type='password' placeholder='New password'><br/><input name='confirmPasswd' maxlength='32' type='password' placeholder='Confirm password'><br/>";
const char HTTP_CONFIG_END[] PROGMEM = "{custom-arg}<br/><button type='submit'>save</button><br/><br/></form>";

const char HTTP_INFO[] PROGMEM = "<br/>{info}";
const char HTTP_EDIT[] PROGMEM = "<br/>{info}<br/><br/><button onclick=\"goBack()\">Edit config</button>";
const char HTTP_PORTAL[] PROGMEM = "<br><h2 style=\"text-align: center\">Captive Portal</h2><br/><p>Please visit {ip} in your browser to config this device.</p>";
const char HTTP_HELP[] PROGMEM = "<br/>{helpInfo}";

const char HTTP_UPDATER_INDEX[] PROGMEM = "<form method='POST' action='' enctype='multipart/form-data'><input type='file' name='update'><br/><br/><input type='submit' value='Update'></form>";
const char HTTP_UPDATER_SUCCESS[] PROGMEM = "<h2>Update Success!</h2><br/><h2>Rebooting...</h2>";
const char HTTP_UPDATER_FAILED[] PROGMEM = "<h2>Update failed !</h2><br/><h2>Rebooting...</h2>";
const char HTTP_CUSTOM_ARG[] PROGMEM = "{arg-label}<input name='{arg-name}' maxlength='{arg-length}' type='{arg-type}' placeholder='{arg-place-holder}' {arg-addition}><br/>";
#endif
