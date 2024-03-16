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

template <typename T> void debug (T text) {
  Serial.print("*DOMO: ");
  Serial.println(text);
}

template <typename T, typename U> void debug (T key, U value) {
  Serial.print("*DOMO: ");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(value);
}

template <typename T, typename... Args>void debug(const T& key, const Args&... values) {
    Serial.print(key);
    Serial.print(": ");
    Serial.print(", ");
    debugHelper(values...);
    Serial.println();
}