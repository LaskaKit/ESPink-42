/* Complete test of display, touchscreen, backlight, I2C, SD and ADC for LaskaKit ESPink-4.2"
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include "FT6236.h"
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include "sd_test.h"
#include "I2C_test.h"
#include "adc_test.h"
#include "PongGame.h"

#define DISPLAY_POWER_PIN 2 // Epaper power pin
#define DISPLAY_LED 26      // Display backlight pin
#define DISPLAY_LED_PWM 155 // dutyCycle 0-255 last minimum was 15

#define BUTTON_X_SIZE 75
#define BUTTON_Y_SIZE 50
#define BUTTON_Y_POS (display.height() - BUTTON_Y_SIZE)
#define BUTTON_1_X_POS 20
#define BUTTON_2_X_POS (BUTTON_1_X_POS + BUTTON_X_SIZE + 20)
#define BUTTON_3_X_POS (BUTTON_2_X_POS + BUTTON_X_SIZE + 20)
#define BUTTON_4_X_POS (BUTTON_3_X_POS + BUTTON_X_SIZE + 20)

// E-paper display
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); //GDEY042T81 (GDEY042T81-FT02), 400x300, SSD1683 (no inking)
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEQ042Z21 400x300, UC8276
// Touch screen
FT6236 ts = FT6236(display.width(), display.height()); // Create object for Touch library

// button enumerator
typedef enum
{
  BUTTON_NONE,
  SD_TEST,
  I2C_SCANNER,
  BACKLIGHT,
  DISPLAY_TEST
} button_t;

// Print text in the middle of coordinates
void print_middle_text(const char *text, uint16_t xmidpos, uint16_t ymidpos, uint16_t color)
{
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.setTextColor(color);
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor((xmidpos - (tbw / 2)), (ymidpos + (tbh / 2)));
  display.print(text);
}

void Display_init()
{
  char text[30];
  display.init();
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.display(false);                                            // Necessary to properly clear display after reset button (V2.2)
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
  display.display(true);
  ts.begin(40, 21, 22); // Init touch screen
  ts.setRotation(2);
}

void Display_drawStartScreen()
{
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&TomThumb);
  display.setTextSize(2);

  display.fillRoundRect(BUTTON_1_X_POS, BUTTON_Y_POS, BUTTON_X_SIZE, BUTTON_Y_SIZE, 2, GxEPD_BLACK);
  print_middle_text("SD", BUTTON_1_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.25), GxEPD_WHITE);
  print_middle_text("test", BUTTON_1_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.75), GxEPD_WHITE);

  display.fillRoundRect(BUTTON_2_X_POS, BUTTON_Y_POS, BUTTON_X_SIZE, BUTTON_Y_SIZE, 2, GxEPD_BLACK);
  print_middle_text("I2C", BUTTON_2_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.25), GxEPD_WHITE);
  print_middle_text("scanner", BUTTON_2_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.75), GxEPD_WHITE);

  display.fillRoundRect(BUTTON_3_X_POS, BUTTON_Y_POS, BUTTON_X_SIZE, BUTTON_Y_SIZE, 2, GxEPD_BLACK);
  print_middle_text("Backlight", BUTTON_3_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.5), GxEPD_WHITE);

  display.fillRoundRect(BUTTON_4_X_POS, BUTTON_Y_POS, BUTTON_X_SIZE, BUTTON_Y_SIZE, 2, GxEPD_BLACK);
  print_middle_text("Display", BUTTON_4_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.25), GxEPD_WHITE);
  print_middle_text("test", BUTTON_4_X_POS + (BUTTON_X_SIZE / 2), BUTTON_Y_POS + (BUTTON_Y_SIZE * 0.75), GxEPD_WHITE);

  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  display.setCursor(5, 20);
  display.print("X:");
  display.setCursor(5, 50);
  display.print("Y:");

  adc_test(display); // Print ADC values
}

void Display_drawTouchPoint(uint16_t x, uint16_t y)
{
  static uint16_t prevx, prevy;
  char text[10];

  // Clear previous text
  display.setTextSize(3);
  display.setTextColor(GxEPD_WHITE);
  sprintf(text, "X: %d", prevx);
  display.setCursor(5, 20);
  display.print(text);
  sprintf(text, "Y: %d", prevy);
  display.setCursor(5, 50);
  display.print(text);

  // Print new text
  display.setTextColor(GxEPD_BLACK);
  sprintf(text, "X: %d", x);
  display.setCursor(5, 20);
  display.print(text);
  sprintf(text, "Y: %d", y);
  display.setCursor(5, 50);
  display.print(text);

  // Draw new point
  display.fillCircle(prevx, prevy, 4, GxEPD_WHITE);
  display.fillCircle(x, y, 4, GxEPD_BLACK);
  prevx = x;
  prevy = y;
  display.display(true);
}

// Get button from touch coordinates
button_t Display_getButton(uint16_t x, uint16_t y)
{
  if ((x > BUTTON_1_X_POS) && (x < (BUTTON_1_X_POS + BUTTON_X_SIZE)))
  {
    return SD_TEST;
  }
  else if ((x > BUTTON_2_X_POS) && (x < (BUTTON_2_X_POS + BUTTON_X_SIZE)))
  {
    return I2C_SCANNER;
  }
  else if ((x > BUTTON_3_X_POS) && (x < (BUTTON_3_X_POS + BUTTON_X_SIZE)))
  {
    return BACKLIGHT;
  }
  else if ((x > BUTTON_4_X_POS) && (x < (BUTTON_4_X_POS + BUTTON_X_SIZE)))
  {
    return DISPLAY_TEST;
  }
  return BUTTON_NONE;
}

void Display_mainScreen()
{
  if (ts.touched())
  {
    // Retrieve a point
    TS_Point p = ts.getPoint();
    if (p.y < BUTTON_Y_POS)
    {
      Display_drawTouchPoint(p.x, p.y);
    }
    else
    {
      button_t button = Display_getButton(p.x, p.y);
      switch (button)
      {
      case SD_TEST:
        Serial.println("SD test");
        SDTest(display);
        while (!ts.touched()) // Wait for touch
        {
        }
        break;
      case I2C_SCANNER:
        Serial.println("I2C scanner");
        I2C_test(display);
        display.display(true);
        ts.begin(40, 21, 22); // Init touch screen
        while (!ts.touched()) // Wait for touch
        {
        }
        break;
      case BACKLIGHT:
        Serial.println("Backlight");
        if (ledcRead(1))
        {
          ledcWrite(1, 0);
        }
        else
        {
          ledcWrite(1, DISPLAY_LED_PWM);
        }
        break;
      case DISPLAY_TEST:
        Serial.println("Display test");
        PongGame_init(display);
        display.display(true);
        while (!ts.touched()) // Wait for touch
        {
          PongGame(display);
          display.display(true);
        }
        break;
      }
      Display_drawStartScreen();
      adc_test(display); // Print ADC values, only when touched
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(DISPLAY_POWER_PIN, OUTPUT);    // Set display power pin as output
  digitalWrite(DISPLAY_POWER_PIN, HIGH); // Turn on the display power

  // configure backlight LED PWM functionalitites
  ledcSetup(1, 1000, 8);         // ledChannel, freq, resolution
  ledcAttachPin(DISPLAY_LED, 1); // ledPin, ledChannel
  ledcWrite(1, 0);               // dutyCycle 0-255

  delay(100); // Delay so it has time to turn on

  adc_init();
  Display_init();
  Display_drawStartScreen();
  display.display(true);
}

void loop()
{
  Display_mainScreen();
}