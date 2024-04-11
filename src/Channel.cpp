#include "Channel.h"
#include "Logger.h"
#include "Utils.h"

Channel::Channel(const char* id, const char* name, uint8_t pin, uint8_t pinMode, bool analog) {
  this->id = id;
  this->pin = pin;
  this->pinMode = pinMode;
  this->analog = analog;
  this->timerControl = pinMode == OUTPUT ? 0 : timer;
  this->name = new char[CHANNEL_NAME_MAX_LENGTH + 1];
  updateName(name);
}

bool Channel::updateName (const char *name) {
  if (strlen(name) > CHANNEL_NAME_MAX_LENGTH) {
    #ifdef LOGGING
    log("Channel new name too long");
    #endif
    return false;
  }
  Utils::copy(this->name, name, CHANNEL_NAME_MAX_LENGTH);
  return true;
}

bool Channel::isEnabled () {
  return this->enabled && this->name != NULL && strlen(this->name) > 0;
}

bool Channel::isOutput() {
  return this->pinMode == OUTPUT;
}

bool Channel::isAnalog() {
  return this->analog;
}

bool Channel::checkTimer() {
  if (timeIsUp()) {
    #ifdef LOGGING
    log("Timer in output channel is up", this->name);
    #endif
    this->timerControl = 0;
    write(prevState);
    return true;
  }
  return false; 
}

void Channel::setTimer(uint32_t time) {
  this->timer = time;
}

void Channel::resetTimer() {
  if (this->timer != -1) {
    this->timerControl = millis() + this->timer;
  }
}

bool Channel::timeIsUp() {
  if (this->timer != -1) {
    return this->timerControl > 0 && millis() > this->timerControl;
  }
  return false;
}

void Channel::setEnabled(bool enabled) {
  this->enabled = enabled;
}

void Channel::setAnalog(bool analog) {
  this->analog = analog;
} 

void Channel::write(int value) {
  this->prevState = this->currState;
  if (this->analog) {
    this->currState = value;
    #ifdef LOGGING
    log("Changing channel state to", this->currState);
    #endif
    analogWrite(this->pin, this->currState);
  } else {
    if (value == HIGH) {
      this->resetTimer();
    }
    this->currState = value;
    #ifdef LOGGING
    log("Changing channel state to", this->currState == HIGH ? "[ON]" : "[OFF]");
    #endif
    digitalWrite(this->pin, this->currState == LOW ? HIGH : LOW);
  }
}

int Channel::getPhysicalState() {
  return this->currState;
}

char* Channel::getName() {
  return this->name;
}

const char* Channel::getId() {
  return this->id;
}

unsigned long Channel::getTimer() {
  return this->timer;
}

int Channel::getPrevState() {
  return this->prevState;
}

void Channel::setState(int state) {
  this->currState = state;
}

bool Channel::read() {
  this->prevState = this->currState;
  if (this->timeIsUp()) {
    if (this->analog) {
      this->currState = analogRead(this->pin);
    } else {
      int read = digitalRead(this->pin);
      read = read == LOW ? HIGH : LOW;
      this->currState = read;
    }
    this->resetTimer();
    return true;
  }
  return false;
}