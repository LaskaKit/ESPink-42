// Touch example with LED baclight testing.
// ESPink-42 Touch FT6336

#include <Arduino.h>
#include "FT6236.h"
#include <GxEPD2_BW.h>
#include <esp_timer.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define DISPLAY_POWER_PIN 2  // Epaper power pin
#define DISPLAY_LED 26       // Display backlight pin
#define DISPLAY_LED_PWM 155  // dutyCycle 0-255 last minimum was 15
#define DIPLAY_BACKLIGHT_ON_TIME  5 // Time for which is backlight on after touch

//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=D8*/ SS, /*DC=D3*/ 17, /*RST=D4*/ 16, /*BUSY=D2*/ 4));  //GDEW042T2FT02, GDEY042T91, 400x300, SSD1683 (no inking)


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
  Serial.begin(115200);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);     // Set display power pin as output
  digitalWrite(DISPLAY_POWER_PIN, HIGH);  // Turn on the display

  // configure backlight LED PWM functionalitites
  ledcSetup(1, 1000, 8);          // ledChannel, freq, resolution
  ledcAttachPin(DISPLAY_LED, 1);  // ledPin, ledChannel
  ledcWrite(1, 0);                // dutyCycle 0-255
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
  ledcWrite(1, 0);  // dutyCycle 0-255
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

    ledcWrite(1, DISPLAY_LED_PWM);  // dutyCycle 0-255
    timer_start();
  }
  // Debouncing. To avoid returning the same touch multiple times you can play with this delay.
  delay(50);
}