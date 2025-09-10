/* Display test for LaskaKit ESPink-4.2"
 * 
 * Board:   LaskaKit ESPink-4.2   https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna
 *
 * Libraries:
 * EPD library: https://github.com/ZinggJM/GxEPD2
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include "bitmaps.h"
#include "OpenSansSB_40px.h"

// #define SLEEP                    // Uncomment so board goes to sleep after printing on display
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 10          // Time ESP32 will go to sleep (in seconds)

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
//GxEPD2_4C<GxEPD2_420c_GDEY0420F51, GxEPD2_420c_GDEY0420F51::HEIGHT> display(GxEPD2_420c_GDEY0420F51(SS, DC, RST, BUSY)); // GDEY0420F51 400x300, HX8717 (no inking)
//GxEPD2_3C<GxEPD2_420c_GDEY042Z98, GxEPD2_420c_GDEY042Z98::HEIGHT> display(GxEPD2_420c_GDEY042Z98(SS, DC, RST, BUSY)); // GDEY042Z98 400x300, SSD1683 (no inking)

void setup()
{
  pinMode(POWER, OUTPUT);    // Set epaper transistor as output
  digitalWrite(POWER, HIGH); // Surn on epaper transistor
  delay(100);            // Delay so it has time to turn on
  display.init();
  display.setRotation(0);
  display.firstPage();
  display.setTextColor(GxEPD_BLACK); // Black font
  display.setCursor(/*x*/ 50, /*y*/ 50);
  display.setFont(&OpenSansSB_40px);
  display.println("Bastliri bastli :-) ");
  display.drawBitmap(0, 100, laskakit, 400, 102, GxEPD_BLACK); // Laskakit logo
  display.display(false);
  delay(1000);
  display.fillScreen(GxEPD_WHITE);  // white background
  display.display(false);
}

void loop()
{
}