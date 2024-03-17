#include "ESP32Domotic.h"
#include "ConfigRepo.h"
#include "Logger.h"
#include <WiFiManager.h>

//Setup y loop para que no chille el compilador
#ifdef DEV_LOCAL_RUN
void setup(){}
void loop(){}
#endif 

static bool needToSaveConfig = false;

//TODO chequear si funciona meter esto en la clase como static
void saveConfigCallback(){
  Serial.println("[CALLBACK] saveConfigCallback fired");
  needToSaveConfig = true;
}

ESP32Domotic::~ESP32Domotic(){}

void ESP32Domotic::init() { 
  ConfigRepo repo;
  repo.init();
  repo.load(this->config);
  if (connectWifi()) {
    Serial.println("Connected to wifi");
    if (needToSaveConfig) {
      repo.save(this->config);
    }
  } else {
    Serial.println("Could not connect to wifi");
    ESP.restart();
  }
}

bool ESP32Domotic::connectWifi() {
  WiFiManagerParameter server("server", "mqtt server", this->config->getMqttHost(), 16);
  WiFiManagerParameter port("port", "mqtt port", this->config->getMqttPort(), 6);
  WiFiManagerParameter name("name", "module name", this->config->getModuleName(), 32);
  WiFiManagerParameter location("location", "module location", this->config->getModuleLocation(), 32);

  WiFiManager wm;
  wm.addParameter(&server);
  wm.addParameter(&port);
  wm.addParameter(&name);
  wm.addParameter(&location);

  wm.setMinimumSignalQuality(40); // set min RSSI (percentage) to show in scans, null = 8%
  wm.setConfigPortalTimeout(this->configPortalTimeout);// auto close configportal after n seconds
  wm.setConnectTimeout(this->wifiConnectTimeout);// how long to try to connect for before continuing
  wm.setSaveConfigCallback(saveConfigCallback);
  wm.setAPClientCheck(true); // avoid timeout if client connected to softap
  wm.setCaptivePortalEnable(true); // disable captive portal redirection

  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  //std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  //wm.setMenu(menu);

  // set dark theme
  //wm.setClass("invert");

  //set static ip
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always
  
  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  bool connected = this->apSsid ? wm.autoConnect(this->apSsid) : wm.autoConnect();
  if (connected) {
    strcpy(this->config->getMqttHost(), server.getValue());
    strcpy(this->config->getMqttPort(), port.getValue());
    strcpy(this->config->getModuleName(), name.getValue());
    strcpy(this->config->getModuleLocation(), location.getValue());
    return true;
  }
  return false;
}

void ESP32Domotic::cycle() {
}

bool ESP32Domotic::addChannel(Channel *channel) {
  if (this->channelsCount < MAX_CHANNELS) {
    this->channels[this->channelsCount++] = channel;
    #ifdef LOGGING
    log("Channel added", channel->getName());
    #endif
    return true;
  } else {
    #ifdef LOGGING
    log("No more channels suported");
    #endif
    return false;
  }
}

void ESP32Domotic::setModuleType(const char* type) {
  this->moduleType = type;
}

ConfigDef* ESP32Domotic::getConfig() {
  return this->config;
}

void ESP32Domotic::setWifiConnectTimeout (uint16_t seconds) {
  this->wifiConnectTimeout = seconds;
}

void ESP32Domotic::setConfigPortalTimeout (uint16_t seconds) {
  this->configPortalTimeout = seconds;
}

void ESP32Domotic::setConfigFileSize (uint16_t bytes) {
  this->configFileSize = bytes;
}

void ESP32Domotic::setPortalSSID (const char* ssid) {
  this->apSsid = ssid;
}