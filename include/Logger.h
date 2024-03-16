#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

void log(const char* text);

template <typename T, typename U> void log(T key, U value);

template <typename T, typename... Args>void log(const T& key, const Args&... values);

#endif