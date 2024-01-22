#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include <Wire.h>

#define TEXT_PADDING 25

static void print_middle_line(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display, char *text, uint16_t padding)
{
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  existing_display.setFont(&TomThumb);
  existing_display.setTextSize(3);
  existing_display.setTextColor(GxEPD_BLACK);
  existing_display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  existing_display.setCursor((existing_display.width() - tbw) / 2, padding);
  existing_display.print(text);
}

void I2C_test(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display)
{
  byte error, address;
  int nDevices = 0;
  char text[40];
  existing_display.fillScreen(GxEPD_WHITE);
  sprintf(text, "Scanning for I2C devices...");
  print_middle_line(existing_display, text, TEXT_PADDING);
  Wire.begin();
  for (address = 0x01; address < 0x7f; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      nDevices++;
      sprintf(text, "I2C device found at address 0x%d", address);
      print_middle_line(existing_display, text, TEXT_PADDING * nDevices + TEXT_PADDING);
    }
    else if (error != 2)
    {
      if (nDevices)
      {
        sprintf(text, "Error %d at address 0x%d", error, address);
        print_middle_line(existing_display, text, TEXT_PADDING * nDevices + 2 * TEXT_PADDING);
      }
      else
      {
        sprintf(text, "Error %d at address 0x%d", error, address);
        print_middle_line(existing_display, text, TEXT_PADDING * 2);
      }
    }
  }
  if (nDevices == 0)
  {
    sprintf(text, "No I2C devices found");
    print_middle_line(existing_display, text, TEXT_PADDING * 2);
  }
  Wire.end();
}