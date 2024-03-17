#include "OTAUpdate.h"
#include "Logger.h"

unsigned long ota_progress_millis = 0;

void onOTAStart() {
  log("OTA update started!");
}

void onOTAProgress(size_t current, size_t final) {
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    #ifdef LOGGING
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
    #endif
  }
}

void onOTAEnd(bool success) {
  if (success) {
    log("OTA update finished successfully!");
  } else {
    log("There was an error during OTA update!");
  }
}

void OTAUpdate::init(){
/*
  this->server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });
*/
  ElegantOTA.begin(&this->server); 
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  this->server.begin();
  log("HTTP server started");
}

void OTAUpdate::handle(){
  this->server.handleClient();
  ElegantOTA.loop();
}
