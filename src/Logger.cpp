#include "Logger.h"

void serialPrintValue(const char* value) {
    Serial.print(value);
}

template <typename T>void serialPrintValue(const T& value) {
    Serial.print(value);
}

template <typename T>void logHelper(const T& value) {
    serialPrintValue(value);
}

void log(const char* text) {
  Serial.print("*DOM: ");
  Serial.println(text);
}

template <typename T, typename U> void log(T key, U value) {
  Serial.print("*DOM: ");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(value);
}

template <typename T, typename... Args>void log(const T& key, const Args&... values) {
    Serial.print("*DOM: ");
    Serial.print(key);
    Serial.print(": ");
    Serial.print(", ");
    logHelper(values...);
    Serial.println();
}