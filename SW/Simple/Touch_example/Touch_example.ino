#include <Arduino.h>
#include "FT6236.h"
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define DISPLAY_POWER_PIN 2 // Epaper power pin

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)
FT6236 ts = FT6236(400, 300);

void display_init()
{
  char text[30];
  display.init();
  display.setRotation(0);
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
  display.fillScreen(GxEPD_WHITE);
  display.display(true);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  sprintf(text, "X:");
  display.setCursor(5, 20);
  display.print(text);
  sprintf(text, "Y:");
  display.setCursor(5, 50);
  display.print(text);
  display.display(true);
}

void setup()
{
  Serial.begin(115200);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);    // Set epaper transistor as output
  digitalWrite(DISPLAY_POWER_PIN, HIGH); // Turn on epaper transistor
  delay(100);                            // Delay so it has time to turn on
  display_init();
  if (!ts.begin(40)) // 40 in this case represents the sensitivity. Try higer or lower for better response.
  {
    Serial.println("Unable to start the capacitive touchscreen.");
  }
  ts.setRotation(3);
}

void loop()
{
  if (ts.touched())
  {
    char text[30];
    // Retrieve a point
    TS_Point p = ts.getPoint();
    display.fillScreen(GxEPD_WHITE);
    sprintf(text, "X: %d", p.x);
    display.setCursor(5, 20);
    display.print(text);
    sprintf(text, "Y: %d", p.y);
    display.setCursor(5, 50);
    display.print(text);
    display.display(true);
  }
  // Debouncing. To avoid returning the same touch multiple times you can play with this delay.
  delay(50);
}