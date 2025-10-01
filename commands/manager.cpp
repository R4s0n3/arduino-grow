#include <Arduino.h>
#include "manager.h"
#include <string.h>  // for strcmp

void CommandManager::handle(const char* cmd) {
  if (strcmp(cmd, "TOGGLE_PIN6") == 0) {
    digitalWrite(6, !digitalRead(6));
    Serial.println("TOGGLE_PIN6");
  } else if (strcmp(cmd, "QUERY_PIN6") == 0) {
    int state = digitalRead(6);
    Serial.println(state ? "HIGH" : "LOW");
  } else {
    Serial.println("UNKNOWN_COMMAND");
  }
