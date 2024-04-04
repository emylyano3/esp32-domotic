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

void ESP32Domotic::saveConfigCallback() {
  #ifdef LOGGING
  log("Callback saveConfigCallback fired");
  #endif
  needToSaveConfig = true;
}

ESP32Domotic::~ESP32Domotic(){
  delete this->config;
  if (this->otaUpdate != nullptr) {
    delete this->otaUpdate;
  }
  if (this->channelsManager != nullptr) {
    delete this->channelsManager;
  }
}

void ESP32Domotic::init() { 
  #ifdef LOGGING
  log("Initializing domotic module");
  #endif    
  ConfigRepo repo;
  repo.init();
  if (!repo.load(this->config)) {
    #ifdef LOGGING
    log("Setting defaults. Could not load config from", this->config->getConfigFilePath());
    #endif   
    this->config->updateMqttHost("192.168.0.101");
    this->config->updateMqttPort("1883");
    this->config->updateModuleLocation("");
    this->config->updateModuleName("");
  }
  if (connectWifi()) {
    #ifdef LOGGING
    log("Connected to wifi");
    #endif   
    if (needToSaveConfig) {
      repo.save(this->config);
    }
  } else {
    #ifdef LOGGING
    log("Could not connect to wifi");
    #endif   
    ESP.restart();
  }
  initOTAUpdate();
  initChannelManager();
}

void ESP32Domotic::initOTAUpdate() {  
  this->otaUpdate = new OTAUpdate();
  this->otaUpdate->init();
}

void ESP32Domotic::initChannelManager() {
  this->channelsManager = new ChannelManager();
  this->channelsManager->setConfig(this->config);
  this->channelsManager->setChannels(this->channels);
  this->channelsManager->init();
}

void ESP32Domotic::cycle() {
  otaUpdate->handle();
  channelsManager->handle();
}

bool ESP32Domotic::connectWifi() {
  #ifdef LOGGING
  log("Connecting wifi");
  #endif   
  WiFiManagerParameter server("server", "mqtt server", this->config->getMqttHost(), MQTT_HOST_PARAM_LENGTH);
  WiFiManagerParameter port("port", "mqtt port", this->config->getMqttPort(), MQTT_PORT_PARAM_LENGTH);
  WiFiManagerParameter name("name", "module name", this->config->getModuleName(), MODULE_NAME_PARAM_LENGTH);
  WiFiManagerParameter location("location", "module location", this->config->getModuleLocation(), MODULE_LOCATION_PARAM_LENGTH);
  #ifdef LOGGING
  log("Params created");
  #endif   
  WiFiManager wm;
  wm.addParameter(&server);
  wm.addParameter(&port);
  wm.addParameter(&name);
  wm.addParameter(&location);
  #ifdef LOGGING
  log("WM created & Params added");
  #endif   
  wm.setMinimumSignalQuality(40); // set min RSSI (percentage) to show in scans, null = 8%
  wm.setConfigPortalTimeout(this->configPortalTimeout);// auto close configportal after n seconds
  wm.setConnectTimeout(this->wifiConnectTimeout);// how long to try to connect for before continuing
  wm.setSaveConfigCallback(std::bind(&ESP32Domotic::saveConfigCallback,this));
  wm.setAPClientCheck(true); // avoid timeout if client connected to softap
  wm.setCaptivePortalEnable(true); // disable captive portal redirection
  wm.setAPStaticIPConfig(IPAddress(10,10,10,10), IPAddress(10,10,10,10), IPAddress(255,255,255,0)); // set static ip,gw,sn 
  wm.setClass("invert"); // set dark theme

  // custom menu via array or vector
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  //std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  //wm.setMenu(menu);

  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always
  
  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails
  #ifdef LOGGING
  log("WM settings done. Launching autoconnect");
  #endif   
  bool connected = this->apSsid ? wm.autoConnect(this->apSsid) : wm.autoConnect();
  if (connected) {
    this->config->updateMqttHost(server.getValue());
    this->config->updateMqttPort(port.getValue());
    this->config->updateModuleName(name.getValue());
    this->config->updateModuleLocation(location.getValue());
    return true;
  }
  #ifdef LOGGING
  log("Could not connect to wifi");
  #endif   
  return false;
}

bool ESP32Domotic::addChannel(Channel *channel) {
  if (this->channels.size() < MAX_CHANNELS) {
    this->channels.push_back(*channel);
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
  this->config->setModuleType(type);
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