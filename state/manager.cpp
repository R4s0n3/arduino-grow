#include "manager.h"

EventState determineState(const SensorData& data) {
  if (!data.valid) return TEST_DONE;
  
  if (data.temperature < TEMP_MIN) return TEMP_LOW;
  if (data.temperature > TEMP_MAX) return TEMP_HIGH;
  if (data.humidity < HUM_MIN) return HUM_LOW;
  if (data.humidity > HUM_MAX) return HUM_HIGH;
  
  return NORMAL;
}

void getDisplayValues(EventState state, const SensorData& data,
                      int& temp, int& hum) {
  temp = data.temperature;
  hum = data.humidity;
  
  switch (state) {
    case TEMP_LOW:  temp = 15; break;
    case TEMP_HIGH: temp = 35; break;
    case HUM_LOW:   hum = 20; break;
    case HUM_HIGH:  hum = 90; break;
    default: break;
  }
}
