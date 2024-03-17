#include "OTAUpdate.h"
#include "Logger.h"

unsigned long ota_progress_millis = 0;

void OTAUpdate::onStart() {
  log("OTA update started!");
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
    log("OTA update finished successfully!");
  } else {
    log("There was an error during OTA update!");
  }
}

void OTAUpdate::init(){
  this->server.on("/", [this]() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });
  ElegantOTA.begin(&this->server); 
  ElegantOTA.onStart(onStart);
  ElegantOTA.onProgress(onProgress);
  ElegantOTA.onEnd(onEnd);

  this->server.begin();
  log("HTTP server started");
}

void OTAUpdate::handle(){
  this->server.handleClient();
  ElegantOTA.loop();
}
