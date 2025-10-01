#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "../config.h"

struct SensorData {
  int temperature;
  int humidity;
  bool valid;
};

EventState determineState(const SensorData& data);
void getDisplayValues(EventState state, const SensorData& data, 
                      int& temp, int& hum);

#endif
