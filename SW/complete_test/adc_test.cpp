#include <Arduino.h>
#include <ESP32AnalogRead.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include "adc_test.h"

// ADC settings
ESP32AnalogRead adc;
#define DIVIDER_RATIO 1.7693877551 // Voltage devider ratio on ADC pin 1MOhm + 1.3MOhm
#define ADC 34                     // ADC pin

void adc_init()
{
  adc.attach(ADC);
}

void adc_test(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display)
{
  char text[10];
	static char old_text[10];
  existing_display.setFont(&TomThumb);
  existing_display.setTextSize(3);
  existing_display.setCursor(existing_display.width() - 60, 20);
	existing_display.setTextColor(GxEPD_WHITE);
	existing_display.print(old_text);
	existing_display.setCursor(existing_display.width() - 60, 20);
	existing_display.setTextColor(GxEPD_BLACK);
	sprintf(text, "%0.2f V", (adc.readVoltage() * DIVIDER_RATIO));
	existing_display.print(text);
}