#include <Arduino.h>
#include <ESP32Domotic.h>
#include <Channel.h>

ESP32Domotic  domoticModule;
Channel       channel("c1", "channel_1", 1, OUTPUT, false, true);

void setup() {
  channel.setState(LOW);
  channel.setTimer(20);
  domoticModule.setModuleType("switch");
  domoticModule.setWifiConnectTimeout(20);
  domoticModule.setConfigPortalTimeout(120);
  domoticModule.addChannel(&channel);
  domoticModule.init();

  channel.isAnalog();
  channel.isOutput();
}

void loop() {
  domoticModule.cycle();
  channel.getPrevState();
  channel.getRawState();
  channel.getMappedState();
}