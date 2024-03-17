#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

template <typename T>void print(const T& value) {
  Serial.print(value);
}

template <typename T> void log(const T& text) {
  #ifdef LOGGING
  Serial.print("*DOM: ");
  Serial.println(text);
  #endif
}

template <typename T, typename U> void log(const T& key, const U& value) {
  #ifdef LOGGING
  Serial.print("*DOM: ");
  Serial.print(key);
  Serial.print(": ");
  Serial.println(value);
  #endif
}

template <typename T, typename... Args>void log(const T& key, const Args&... values) {
  #ifdef LOGGING
  Serial.print("*DOM: ");
  Serial.print(key);
  Serial.print(": ");
  Serial.print(", ");
  print(values...);
  Serial.println();
  #endif
}
#endif