#ifndef RAIN_METER_H
#define RAIN_METER_H

#include <Arduino.h>

class RainMeter {
private:
  int analogPin;
  int digitalPin;

public:
  RainMeter(int analogPin, int digitalPin);
  void begin();
  bool isRaining();
  int rainIntensity();
};

#endif
