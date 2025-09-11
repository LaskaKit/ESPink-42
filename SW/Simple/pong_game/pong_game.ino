/* Pong game example for LaskaKit ESPink-4.2"
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include "PongGame.h"

//#define ESPink42_V2     //for version v2.4 and earlier
#define ESPink42_V3     //for version v3.0 and above

#ifdef ESPink42_V2
  //MOSI/SDI    23
  //CLK/SCK     18
  //SS/CS       5
  #define DC    17 
  #define RST   16  
  #define BUSY  4 
  #define POWER 2
  #define SDA   21
  #define SCL   22
  #define BAT   34
  #define DISPLAY_LED 26      // Display backlight pin
#else ESPink42_V3
  //MOSI/SDI    11
  //CLK/SCK     12
  //SS/CS       10
  #define DC    48 
  #define RST   45  
  #define BUSY  38 
  #define POWER 47
  #define SDA   42
  #define SCL   2
  #define BAT   9
  #define DISPLAY_LED 1      // Display backlight pin
#endif

// E-paper display
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(SS, DC, RST, BUSY)); //GDEY042T81 (GDEY042T81-FT02), 400x300, SSD1683 (no inking)

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
  // turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the Display on (HIGH is the voltage level)
  delay(500);   
  Display_init();
}

void loop() {
  PongGame(display);
  display.display(true);
}
