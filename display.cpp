#include "display.h"
#include <Wire.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
