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
  // if (Utils::equals(this->name, name)) {
    // #ifdef LOGGING
    // log("Channel new name same as previous");
    // #endif
  //   return false;
  // }
  Utils::copy(this->name, name, CHANNEL_NAME_MAX_LENGTH);
  return true;
}

void Channel::resetTimerControl() {
  this->timerControl = millis() + this->timer;
}

bool Channel::timeIsUp() {
  return this->timerControl > 0 && millis() > this->timerControl;
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

void Channel::setTimer(uint32_t time) {
  this->timer = time;
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
    if (value == LOW) {
      this->resetTimerControl();
    }
    this->currState = value;
    #ifdef LOGGING
    log("Changing channel state to", this->currState == HIGH ? "[ON]" : "[OFF]");
    #endif
    digitalWrite(this->pin, this->currState);
  }
}

const char* Channel::getLogicState() {
  //TODO Mapear este estado de acuerdo a la funcion que este definida
  // deberia manejar la logica invertida (si aplica)
  return getPhysicalState() == LOW ? "1" : "0";
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
    this->resetTimerControl();
    return true;
  }
  return false;
}