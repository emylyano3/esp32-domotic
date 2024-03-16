#include <Arduino.h>
#include <ESP32Domotic.h>
#include <Channel.h>
#include <Logger.h>

ESP32Domotic  domoticModule;
Channel       channel("c1", "channel_1", 1, OUTPUT, false, true);

void setup() {
  log("Switch example setup");
  channel.setState(LOW);
  channel.setTimer(20);
  domoticModule.setModuleType("switch");
  domoticModule.setWifiConnectTimeout(20);
  domoticModule.setConfigPortalTimeout(120);
  domoticModule.addChannel(&channel);
  domoticModule.init();

  log("Channel is analog", channel.isAnalog());
  log("Channel is output", channel.isOutput());
}

void loop() {
  domoticModule.cycle();
  log("Channel previous state", channel.getPrevState());
  log("Channel current raw state", channel.getRawState());
  log("Channel current mapped state", channel.getMappedState());
  delay(100);
}