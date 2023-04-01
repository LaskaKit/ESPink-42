#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Wire.h>

#define TIME_TO_SLEEP 5           // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

#define DISPLAY_POWER_PIN 2 // Epaper power pin

#define TEXT_PADDING 30

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)

void start_sleep()
{
  gpio_hold_en((gpio_num_t)DISPLAY_POWER_PIN); // Hold Epaper on during sleep
  gpio_deep_sleep_hold_en();                   // Hold Epaper on during sleep
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
  display.print(text);
}

void I2C_test()
{
  byte error, address;
  int nDevices = 0;
  char text[40];
  display.fillScreen(GxEPD_WHITE);
  sprintf(text, "Scanning for I2C devices...");
  print_middle_line(text, TEXT_PADDING);
  for (address = 0x01; address < 0x7f; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      nDevices++;
      sprintf(text, "I2C device found at address 0x%d", address);
      print_middle_line(text, TEXT_PADDING);
    }
    else if (error != 2)
    {
      if (nDevices)
      {
        sprintf(text, "Error %d at address 0x%d", error, address);
        print_middle_line(text, TEXT_PADDING * nDevices + 2 * TEXT_PADDING);
      }
      else
      {
        sprintf(text, "Error %d at address 0x%d", error, address);
        print_middle_line(text, TEXT_PADDING * 2);
      }
    }
  }
  if (nDevices == 0)
  {
    sprintf(text, "No I2C devices found");
    print_middle_line(text, TEXT_PADDING * 2);
    sprintf(text, "Next scanning in 5 seconds...");
    print_middle_line(text, TEXT_PADDING * 3);
    sprintf(text, "Went to sleep %d times", bootCount);
    print_middle_line(text, TEXT_PADDING * 4);
    sprintf(text, "Going to sleep");
    print_middle_line(text, TEXT_PADDING * 5);
  }
  else
  {
    sprintf(text, "Next scanning in 5 seconds...");
    print_middle_line(text, TEXT_PADDING * nDevices + 2 * TEXT_PADDING);
    sprintf(text, "Went to sleep %d times", bootCount);
    print_middle_line(text, TEXT_PADDING * 3);
    sprintf(text, "Going to sleep");
    print_middle_line(text, TEXT_PADDING * nDevices + 4 * TEXT_PADDING);
  }
  display.display(true);
}

void setup()
{
  Serial.begin(115200);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);    // Set epaper transistor as output
  digitalWrite(DISPLAY_POWER_PIN, HIGH); // Turn on epaper transistor
  delay(100);                            // Delay so it has time to turn on
  if (bootCount)                         // If first boot, redraw
  {
    display.init(0, false);
  }
  else
  {
    display.init(0, true);
  }
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
  Wire.begin();
  I2C_test();
  bootCount++; // Increment boot number every reboot
  start_sleep();
}

void loop()
{
}