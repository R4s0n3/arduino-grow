#include "manager.h"
#include <Arduino.h>

void setupLEDs() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
}

void updateLEDs(EventState state) {
  if (state == TEMP_LOW || state == TEMP_HIGH || 
      state == HUM_LOW || state == HUM_HIGH) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  } else if (state == TEST_DONE || state == NORMAL) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }
}
