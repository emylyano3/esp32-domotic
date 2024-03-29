#include "Channel.h"

Channel::Channel(const char* id, const char* name, uint8_t pin, uint8_t pinMode, bool analog, bool inverted) {
  this->id = id;
  this->name = new char[CHANNEL_NAME_MAX_LENGTH + 1];
  updateName(name);
  this->pin = pin;
  this->pinMode = pinMode;
  this->analog = analog;
  this->inverted = inverted;
  this->timerControl = pinMode == OUTPUT ? 0 : timer;
}

void Channel::updateName (const char *v) {
  String(v).toCharArray(this->name, CHANNEL_NAME_MAX_LENGTH);
}

void Channel::updateTimerControl() {
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

void Channel::setAnalog(bool analog) {
  this->analog = analog;
} 

void Channel::write(int value) {
  this->prevState = this->currState;
  if (this->analog) {
    this->currState = value;
    #ifdef LOGGING
    // debug(F("Changing channel state to"), this->currState);
    #endif
    analogWrite(this->pin, this->currState);
  } else {
    if (value == HIGH) {
      this->updateTimerControl();
    }
    if (this->inverted) {
      value = value == LOW ? HIGH : LOW;
    }
    this->currState = value;
    #ifdef LOGGING
    // debug(F("Changing channel state to"), this->currState == HIGH ? "[ON]" : "[OFF]");
    #endif
    digitalWrite(this->pin, this->currState);
  }
}

void Channel::setStateMapper(std::function<int(int)> mapper) {
  this->valueMapper = mapper;
}

int Channel::getMappedState() {
  if (this->valueMapper) {
    return this->valueMapper(this->currState);
  }
  return this->currState;
}

int Channel::getRawState() {
  return this->currState;
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
      if (this->inverted) {
        read = read == LOW ? HIGH : LOW;
      }
      this->currState = read;
    }
    this->updateTimerControl();
    return true;
  }
  return false;
}