#include "Log.h"

void serialPrintValue(const char* value) {
    Serial.print(value);
}

template <typename T>void serialPrintValue(const T& value) {
    Serial.print(value);
}

template <typename T>void debugHelper(const T& value) {
    serialPrintValue(value);
}

void debug (const char* text) {
  Serial.print("*DOM: ");
  Serial.println(text);
}

template <typename T, typename U> void debug (T key, U value) {
  Serial.print("*DOM: ");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(value);
}

template <typename T, typename... Args>void debug(const T& key, const Args&... values) {
    Serial.print("*DOM: ");
    Serial.print(key);
    Serial.print(": ");
    Serial.print(", ");
    debugHelper(values...);
    Serial.println();
}