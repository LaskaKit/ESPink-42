/* ADC test for LaskaKit ESPink-4.2"
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
#include <Fonts/FreeMonoBold12pt7b.h>
#include <driver/rtc_io.h>

#define TIME_TO_SLEEP 5           // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

// ADC settings
#define DIVIDER_RATIO 1.7693877551 // Voltage devider ratio on ADC pin 1MOhm + 1.3MOhm

#define TEXT_PADDING 30

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
void start_sleep()
{
  gpio_hold_en((gpio_num_t)POWER); // Hold Epaper on during sleep
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
  sprintf(text, "Battery voltage is %0.2f V", (analogReadMilliVolts(BAT) * DIVIDER_RATIO / 1000));
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
  pinMode(POWER, OUTPUT);    // Set epaper transistor as output
  digitalWrite(POWER, HIGH); // Turn on epaper transistor
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