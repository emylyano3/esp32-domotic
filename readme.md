## Domotic library for ESP 32

Domotic library for ESP32 platform helpful to write code for domotic modules that are intended to:
- Have channels
- Update through OTA
- Persist & load configuration
- Connect over MQTT

This library has a specific purpose and is not intented to be generic.
It's goal is to simplify and reduce the code necessary to write basic domotic modules (like switches or sensors), capable of:
- being easily firmware updated
- manage one or more channels 
- connect to a central system using a generic protocol like MQTT
- persist module and channels configuration

**To compile project in PlatformIO CLI:**

> pio ci ./examples/* -c platformio.ini --lib=. -e dev_ci

> pio run -e dev_local_run