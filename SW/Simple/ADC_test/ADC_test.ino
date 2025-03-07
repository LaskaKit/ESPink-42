/* ADC test for LaskaKit ESPink-4.2"
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
#include <Fonts/FreeMonoBold12pt7b.h>
#include <driver/rtc_io.h>

#define TIME_TO_SLEEP 5           // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

#define DISPLAY_POWER_PIN 2 // Epaper power pin

// ADC settings
#define DIVIDER_RATIO 1.7693877551 // Voltage devider ratio on ADC pin 1MOhm + 1.3MOhm
#define ADC 34

#define TEXT_PADDING 30

//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); //GDEY042T81, 400x300, SSD1683 (no inking)
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEQ042Z21 400x300, UC8276

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
  display.println(text);
}

void display_text()
{
  char text[40];
  display.fillScreen(GxEPD_WHITE);
  sprintf(text, "Battery voltage is %0.2f V", (analogReadMilliVolts(ADC) * DIVIDER_RATIO / 1000));
  print_middle_line(text, TEXT_PADDING);
  sprintf(text, "Went to sleep %d times", bootCount);
  print_middle_line(text, TEXT_PADDING * 2);
  sprintf(text, "Next wake up in %d seconds", TIME_TO_SLEEP);
  print_middle_line(text, TEXT_PADDING * 3);
  sprintf(text, "Going to sleep");
  print_middle_line(text, TEXT_PADDING * 4);
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
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
  display_text();
  bootCount++; // Increment boot number every reboot
  start_sleep();
}

void loop()
{
}