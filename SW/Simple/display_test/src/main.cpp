#include <Arduino.h>
#include <GxEPD2_BW.h>
#include "bitmaps.h"
#include "OpenSansSB_40px.h"

//#define SLEEP                    // Uncomment so board goes to sleep after printing on display
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10          // Time ESP32 will go to sleep (in seconds)

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEW042T2 400x300, UC8176 (IL0398)

void start_sleep()
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void setup()
{
  pinMode(2, OUTPUT);     // Set epaper transistor as output
  digitalWrite(2, HIGH);  // Surn on epaper transistor
  delay(100);             // Delay so it has time to turn on
  display.init();
  display.setRotation(0);
  display.firstPage();    
  do
  {
    display.setTextColor(GxEPD_BLACK);  // Black font
    display.setCursor(/*x*/50, /*y*/50);
    display.setFont(&OpenSansSB_40px);
    display.println("Bastliri bastli :-) ");
    display.drawBitmap(0, 100, laskakit, 400, 102, GxEPD_BLACK); // Laskakit logo
  }
  while (display.nextPage());
  digitalWrite(2, LOW); // Turn off epaper transistor
  #ifdef SLEEP          // If SLEEP is uncommented
    start_sleep();
  #endif
}

void loop() 
{
}