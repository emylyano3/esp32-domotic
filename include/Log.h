#ifndef Log_h
#define Log_h

#include <Arduino.h>

void debug (const char* text);

template <typename T, typename U> void debug (T key, U value);

template <typename T, typename... Args>void debug(const T& key, const Args&... values);

#endif