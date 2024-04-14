#include <Arduino.h>
#include <ESP32Domotic.h>
#include <Channel.h>
#include <Logger.h>

ESP32Domotic*  domoticModule  = new ESP32Domotic();
Channel*       channel        = new Channel("c1", "channel_1", 1, OUTPUT, false);

void setup() {
  Serial.begin(115200);
  log("Switch example setup");
  channel->setState(LOW);
  channel->setTimer(20);
  channel->setEnabled(false);
  domoticModule->setModuleType("switch");
  domoticModule->setWifiConnectTimeout(20);
  domoticModule->setConfigPortalTimeout(120);
  domoticModule->setPortalSSID("proeza-domotics-demo");
  domoticModule->addChannel(channel);
  domoticModule->init();

  log("Channel name", channel->getName());
  log("Channel is analog", channel->isAnalog());
  log("Channel is output", channel->isOutput());
  log("Channel is enabled", channel->isEnabled());
  log("Channel timer", channel->getTimer());
  log("Module location", domoticModule->getConfig()->getModuleLocation());
  log("Module name", domoticModule->getConfig()->getModuleName());
}

void loop() {
  domoticModule->cycle();
  log("Channel previous state", channel->getPrevState());
  log("Channel current raw state", channel->getCurrentState());
  delay(100);
}