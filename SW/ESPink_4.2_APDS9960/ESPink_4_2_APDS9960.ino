/***************************************************************************
*
* Example code of gesture sensor for control of eInk.
* By movement to right and left, the weather is changed.
* By movement to up and down, the depature time is changed.
* It is possible to use this example for various ESP32 boards, eInks and various usage
*
* Written by laskakit (2023)
*
* Hardware:
* https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/?variantId=13232
* https://www.laskakit.cz/laskakit-apds-9960-senzor-priblizeni-a-gest/
*
* Libraries:
* https://github.com/ZinggJM/GxEPD2
* https://github.com/adafruit/Adafruit_APDS9960
*
 ***************************************************************************/

// eInk
#include <GxEPD2_BW.h>
#include "OpenSansSB_32px.h"
#include "bitmaps.h"

// APDS sensor
#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

// Pinout ESPink-42 and ESPink
#define PIN_SS 5
#define PIN_DC 17
#define PIN_RST 16
#define PIN_BUSY 4

// eInk definition for ESPink-42 with 4.2" eInk
GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS*/ PIN_SS, /*DC*/ PIN_DC, /*RST*/ PIN_RST, /*BUSY*/ PIN_BUSY));

// Template - time, wheater, departure
String timeH[] = { "0h", "3h", "6h", "9h", "12h", "15h" };
String weather[] = { "0mm", "2mm", "1.5mm", "0.2mm", "0mm", "0.2mm" };
String departure[] = { "9:25", "9:35", "9:50", "10:10", "10:25", "10:40" };

uint8_t horizontal = 0;
uint8_t vertical = 1;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  if (!apds.begin()) {
    Serial.println("failed to initialize device! Please check your wiring.");
  } else Serial.println("Device initialized!");

  //gesture mode will be entered once proximity mode senses something close
  apds.enableProximity(true);
  apds.enableGesture(true);

  // eInk
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(100);
  display.init();
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&OpenSansSB_32px);

  // print default weater and departures
  for (uint8_t depCnt = 0; depCnt < 3; depCnt++) { // departure count - possition
    display.setCursor(/*x*/ 250, /*y*/ 50 + (depCnt * 50));
    display.print(departure[depCnt]);
  }

  for (uint8_t weatCnt = 0; weatCnt < 3; weatCnt++) { // weather count - possition
    display.setCursor(/*x*/ 10 + (120 * weatCnt), /*y*/ 230);
    display.print(timeH[weatCnt]);

    display.setCursor(/*x*/ 10 + (120 * weatCnt), /*y*/ 280);
    display.print(weather[weatCnt]);
  }

  // update eInk
  display.display(false);
  // turn off eInk
  digitalWrite(2, LOW);
}

// the loop function runs over and over again forever
void loop() {
  //read a gesture from the device
  uint8_t gesture = apds.readGesture();
  delay(100);
  if (gesture != NULL) {
    uint8_t space = 0;

    // turn on the eInk
    digitalWrite(2, HIGH);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);

    if (gesture == APDS9960_DOWN) {
      vertical = 0;

      Serial.println("v");
    }
    if (gesture == APDS9960_UP) {
      vertical = 1;

      Serial.println("^");
    }

    if (gesture == APDS9960_LEFT) {
      horizontal = 0;

      Serial.println("<");
    }

    if (gesture == APDS9960_RIGHT) {
      horizontal = 1;

      Serial.println(">");
    }

    space = 0;
    if (horizontal == 0) {
      for (uint8_t weatCnt = 0; weatCnt < 3; weatCnt++) { // weather count - possition
        display.setCursor(/*x*/ 10 + (space * 120), /*y*/ 230);
        display.print(timeH[weatCnt]);

        display.setCursor(/*x*/ 10 + (space * 120), /*y*/ 280);
        display.print(weather[weatCnt]);
        space++;
      }
    } else {
      for (uint8_t weatCnt = 3; weatCnt < 6; weatCnt++) {
        display.setCursor(/*x*/ 10 + (space * 120), /*y*/ 230);
        display.print(timeH[weatCnt]);

        display.setCursor(/*x*/ 10 + (space * 120), /*y*/ 280);
        display.print(weather[weatCnt]);
        space++;
        }
      }

      space = 0;
      if (vertical == 0) {
        for (uint8_t depCnt = 3; depCnt < 6; depCnt++) {  // departure count - possition
          display.setCursor(/*x*/ 250, /*y*/ 50 + (space * 50));
          display.print(departure[depCnt]);

          space++;
        }
      } else {
        for (uint8_t depCnt = 0; depCnt < 3; depCnt++) {
          display.setCursor(/*x*/ 250, /*y*/ 50 + (space * 50));
          display.print(departure[depCnt]);
          space++;
        }
      }

    // update eInk
    display.display(false);
    // turn off eInk
    digitalWrite(2, LOW);
  }
}
