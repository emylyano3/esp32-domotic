#include "OTAUpdate.h"
#include "Logger.h"

unsigned long ota_progress_millis = 0;

void OTAUpdate::onStart() {
  #ifdef LOGGING
  log("OTA update started!");
  #endif
}

void OTAUpdate::onProgress(size_t current, size_t final) {
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    #ifdef LOGGING
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    #endif
  }
}

void OTAUpdate::onEnd(bool success) {
  if (success) {
    #ifdef LOGGING
    log("OTA update finished successfully!");
    #endif
  } else {
    #ifdef LOGGING
    log("There was an error during OTA update!");
    #endif
  }
}

void OTAUpdate::init(){
  #ifdef LOGGING
  log("Initializing OTA Update");
  #endif
  this->server.on("/", [this]() {
    this->server.send(200, "text/plain", "Proeza domotic module");
  });
  ElegantOTA.begin(&this->server); 
  ElegantOTA.onStart(onStart);
  ElegantOTA.onProgress(onProgress);
  ElegantOTA.onEnd(onEnd);
  this->server.begin();
  #ifdef LOGGING
  log("HTTP server started");
  #endif
}

void OTAUpdate::handle(){
  this->server.handleClient();
  ElegantOTA.loop();
}
