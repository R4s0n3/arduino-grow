#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include "../config.h"

bool setupDisplay();
void renderDisplay(EventState activeState, EventState overrideState, 
                   int temp, int hum, bool sensorError);

#endif
