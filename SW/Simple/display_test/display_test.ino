/* Display test for LaskaKit ESPink-4.2"
 *
 * -------- ESPink pinout -------
 * MOSI/SDI 23
 * CLK/SCK 18
 * SS 5 //CS
 * DC 17  
 * RST 16  
 * BUSY 4 
 * -------------------------------
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include "bitmaps.h"
#include "OpenSansSB_40px.h"

// #define SLEEP                    // Uncomment so board goes to sleep after printing on display
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 10          // Time ESP32 will go to sleep (in seconds)

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEQ042Z21 400x300, UC8276


void setup()
{
  pinMode(2, OUTPUT);    // Set epaper transistor as output
  digitalWrite(2, HIGH); // Surn on epaper transistor
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
}

void loop()
{
}