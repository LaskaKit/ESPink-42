/*
ESPD-3.5" - basic test with text and bitmap map in internal memory
Written by laskakit.cz (2022)

Used library: https://github.com/ZinggJM/GxEPD
*/

#include <GxEPD.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include "SPI.h"
#include "bitmaps.h"
#include "OpenSansSB_40px.h"

#include <GxGDEW042T2/GxGDEW042T2.h>    // 4.2" b/w

#define SS 5
#define DC 17
#define RST 16
#define BUSY 4

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ DC, /*RST=*/ RST);
GxEPD_Class display(io, /*RST=*/ RST, /*BUSY=*/ BUSY);

void setup()
{
  pinMode(2, OUTPUT); // ovladani tranzistoru spinajici napajeni ePaperu
  digitalWrite(2, HIGH); // povoleni napajeni ePaper
  delay(100);
  display.init(); // inicializace
  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
}
void loop()
{ 
  display.drawBitmap(laskakit, 0, 100, 400, 102, GxEPD_BLACK); // zobrazeni loga

  display.setTextColor(GxEPD_BLACK);  // black font
  display.setCursor(/*x*/50, /*y*/50);
  display.setFont(&OpenSansSB_40px);
  display.println("Bastliri bastli :-) ");

  display.update(); // aktualizace ePaperu
  delay(100);
  digitalWrite(2, LOW); // odpojeni napajeni ePaperu

  delay(10000);
}
