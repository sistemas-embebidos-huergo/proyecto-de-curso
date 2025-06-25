#include "rain-meter.h"
#include <Arduino.h>

RainMeter::RainMeter(int analogPin, int digitalPin)
: analogPin(analogPin), digitalPin(digitalPin) {}

void RainMeter::begin() {
  pinMode(analogPin, INPUT);
  pinMode(digitalPin, INPUT);
}

bool RainMeter::isRaining() {
  bool rawRead = digitalRead(digitalPin);
  return !rawRead;
}

int RainMeter::rainIntensity() {
  int rawRead = analogRead(analogPin);
  return abs(5 - map(rawRead, 0, 5000, 0, 5));
}
