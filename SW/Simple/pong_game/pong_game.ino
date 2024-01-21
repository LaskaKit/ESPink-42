/* Pong game example for LaskaKit ESPink-4.2"
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include "PongGame.h"

#define DISPLAY_POWER_PIN 2 // Epaper power pin

// E-paper display
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=D8*/ SS, /*DC=D3*/ 17, /*RST=D4*/ 16, /*BUSY=D2*/ 4)); // GDEW042T2FT02, GDEY042T91, 400x300, SSD1683 (no inking)

void Display_init()
{
  char text[30];
  display.init();
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.display(false);                                            // Necessary to properly clear display after reset button (V2.2)
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
  display.display(true);
  PongGame_init(display);
  display.display(true);
}

void setup() {
  Display_init();
}

void loop() {
  PongGame(display);
  display.display(true);
}
