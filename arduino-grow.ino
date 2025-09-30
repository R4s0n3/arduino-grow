#include <DHT11.h>
#include "config.h"
#include "display.h"
#include "leds.h"
#include "state.h"

DHT11 dht(DHTPIN);
EventState overrideState = TEST_DONE;

void setup() {
  Serial.begin(9600);
  
  setupLEDs();
  pinMode(TEST_BUTTON, INPUT_PULLUP);
  
  if (!setupDisplay()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
}

void loop() {
  static unsigned long lastPress = 0;
  
  // Button handling
  if (digitalRead(TEST_BUTTON) == LOW && millis() - lastPress > 400) {
    lastPress = millis();
    
    if (overrideState == TEST_DONE) overrideState = TEMP_LOW;
    else if (overrideState == TEMP_LOW) overrideState = TEMP_HIGH;
    else if (overrideState == TEMP_HIGH) overrideState = HUM_LOW;
    else if (overrideState == HUM_LOW) overrideState = HUM_HIGH;
    else if (overrideState == HUM_HIGH) overrideState = NORMAL;
    else if (overrideState == NORMAL) overrideState = TEST_DONE;
  }

  delay(2000);

  // Read sensor
  SensorData data;
  int result = dht.readTemperatureHumidity(data.temperature, data.humidity);
  data.valid = (result == 0);

  // Determine states
  EventState liveState = determineState(data);
  EventState activeState = 
    (overrideState == TEST_DONE) ? liveState : overrideState;

  // Get display values
  int shownTemp, shownHum;
  getDisplayValues(activeState, data, shownTemp, shownHum);

  // Update outputs
  updateLEDs(activeState);
  renderDisplay(activeState, overrideState, shownTemp, shownHum, !data.valid);
}
