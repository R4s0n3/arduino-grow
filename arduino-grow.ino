#include <DHT11.h>
#include "config.h"
#include "display/manager.h"
#include "leds/manager.h"
#include "state/manager.h"
#include "commands/manager.h"

DHT11 dht(DHTPIN);
EventState overrideState = TEST_DONE;

void setup() {
  Serial.begin(9600);
  
  pinMode(6, OUTPUT);  // For TOGGLE_PIN6 command
  
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


  // LED CONFIG
 String command = Serial.readStringUntil('\n');
 command.trim();

 CommandManager::handle(command.c_str());

  // Get display values
  int shownTemp, shownHum;
  getDisplayValues(activeState, data, shownTemp, shownHum);

  // Update outputs
  updateLEDs(activeState);
  renderDisplay(activeState, overrideState, shownTemp, shownHum, !data.valid);
}

// ============================================================================
// IMPLEMENTATION CODE (moved from subdirectory .cpp files)
// ============================================================================

// Additional includes for implementation
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

// Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ============================================================================
// COMMAND MANAGER IMPLEMENTATION
// ============================================================================

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
}

// ============================================================================
// DISPLAY MANAGER IMPLEMENTATION
// ============================================================================

bool setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    return false;
  }
  display.clearDisplay();
  display.display();
  return true;
}

void renderDisplay(EventState activeState, EventState overrideState,
                   int temp, int hum, bool sensorError) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Headers
  display.setCursor(2, 0);
  display.print("Temp (C)");
  display.setCursor(72, 0);
  display.print("Humidity");
  display.drawLine(0, 18, 127, 18, SSD1306_WHITE);
  display.drawLine(64, 0, 64, 63, SSD1306_WHITE);

  // Left cell (Temp)
  if (activeState == TEMP_LOW || activeState == TEMP_HIGH) {
    display.fillRect(0, 19, 64, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  }
  display.setTextSize(2);
  display.setCursor(2, 30);
  display.print(temp);
  display.print("C");

  // Right cell (Hum)
  if (activeState == HUM_LOW || activeState == HUM_HIGH) {
    display.fillRect(65, 19, 63, 35, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  } else {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  }
  display.setCursor(72, 30);
  display.print(hum);
  display.print("%");

  // Status bar
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.fillRect(0, 55, 128, 9, SSD1306_BLACK);
  display.setCursor(0, 56);

  if (overrideState == TEST_DONE) {
    display.print("LIVE MODE");
    if (sensorError) display.print(" (SENSOR ERR)");
  } else {
    display.print("TEST: ");
    switch (activeState) {
      case TEMP_LOW:  display.print("TEMP_LOW"); break;
      case TEMP_HIGH: display.print("TEMP_HIGH"); break;
      case HUM_LOW:   display.print("HUM_LOW"); break;
      case HUM_HIGH:  display.print("HUM_HIGH"); break;
      case NORMAL:    display.print("NORMAL"); break;
      default:        display.print("UNKNOWN"); break;
    }
  }

  display.display();
}

// ============================================================================
// LEDS MANAGER IMPLEMENTATION
// ============================================================================

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

// ============================================================================
// STATE MANAGER IMPLEMENTATION
// ============================================================================

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
