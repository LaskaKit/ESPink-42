#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define TIME_TO_SLEEP 5           // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

#define DISPLAY_POWER_PIN 2 // Epaper power pin

#define TEXT_PADDING 30

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)

void start_sleep()
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void print_middle_line(char *text, uint16_t padding)
{
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor((display.width() - tbw) / 2, padding);
  display.println(text);
}

void display_text()
{
  char text[40];
  display.fillScreen(GxEPD_WHITE);
  sprintf(text, "Went to sleep %d times", bootCount);
  print_middle_line(text, TEXT_PADDING);
  sprintf(text, "Next wake up in %d seconds", TIME_TO_SLEEP);
  print_middle_line(text, TEXT_PADDING * 2);
  sprintf(text, "Going to sleep");
  print_middle_line(text, TEXT_PADDING * 3);
  display.display(false);
}

void setup()
{
  Serial.begin(115200);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);    // Set epaper transistor as output
  digitalWrite(DISPLAY_POWER_PIN, HIGH); // Surn on epaper transistor
  delay(100);                            // Delay so it has time to turn on
  display.init();
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow(); // Set display window for full update
  //Wire.begin();
  display_text();
  bootCount++; // Increment boot number every reboot
  digitalWrite(DISPLAY_POWER_PIN, LOW); // Turn off epaper transistor
  start_sleep();
}

void loop()
{
}
