/* Touch example with LED baclight testing for LaskaKit ESPink-4.2"
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
#include "FT6236.h"
#include <GxEPD2_BW.h>
#include <esp_timer.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define DISPLAY_LED_PWM 50  // dutyCycle 0-255 last minimum was 15
#define DIPLAY_BACKLIGHT_ON_TIME  5 // Time for which is backlight on after touch

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

FT6236 ts = FT6236(400, 300);

void display_init() {
  char text[30];
  display.init();
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.display(false);                                             // Necessary to properly clear display after reset button (V2.2)
  display.setPartialWindow(0, 0, display.width(), display.height());  // Set display window for fast update
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

void setup() {  
  // configure backlight LED PWM functionalitites
  analogWrite(DISPLAY_LED, 0);      // Set brightness of backlight
  Serial.begin(115200);
  pinMode(POWER, OUTPUT);     // Set display power pin as output
  digitalWrite(POWER, HIGH);  // Turn on the display
  Serial.println("Display power ON");
  delay(500);   
  Wire.begin (SDA, SCL);

  delay(100);                     // Delay so it has time to turn on

  display_init();
  if (!ts.begin(10))  // 40 in this case represents the sensitivity. Try higer or lower for better response.
  {
    Serial.println("Unable to start the capacitive touchscreen.");
  }
  ts.setRotation(2);
}

void timer_start(void) {
  esp_timer_create_args_t one_shot_timer_args = {
    .callback = &on_one_shot_timer,
    .name = "one-shot"
  };
  esp_timer_handle_t one_shot_timer;
  esp_timer_create(&one_shot_timer_args, &one_shot_timer);
  esp_timer_start_once(one_shot_timer, (DIPLAY_BACKLIGHT_ON_TIME * 1000000)); 
}

/* Timer elapsed function */
void on_one_shot_timer(void* arg) {
    analogWrite(DISPLAY_LED, DISPLAY_LED_PWM);      // Set brightness of backlight
}

void loop() {
  if (ts.touched()) {
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

    analogWrite(DISPLAY_LED, DISPLAY_LED_PWM);      // Set brightness of backlight
    timer_start();
  }
  // Debouncing. To avoid returning the same touch multiple times you can play with this delay.
  delay(50);
}