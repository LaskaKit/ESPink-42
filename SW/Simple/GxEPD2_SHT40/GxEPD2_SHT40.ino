/* Display test for LaskaKit ESPink-4.2
 * 
 * Board:   LaskaKit ESPink-4.2   https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna
 *
 * Libraries:
 * SHT40: https://github.com/adafruit/Adafruit_SHT4X
 * EPD library: https://github.com/ZinggJM/GxEPD2
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
 
#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_BW.h>

#include "Adafruit_SHT4x.h"

// Fonts
#include "OpenSansSB_12px.h"
#include "OpenSansSB_50px.h"

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


// SHT40
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

// ADC
float vBat = 0.0;

void setup() {
  Serial.begin(115200);
  // turn on power to display
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, HIGH);   // turn the Display on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(500);   

  Wire.begin (SDA, SCL);

  // read the voltage
  // read ADC and calculate the voltage
  vBat = analogReadMilliVolts(BAT) * 1.769 / 1000; // the ratio of divider, R2=1.3M; R1=1M
  Serial.println((String)"VCELL V   : "+ vBat + "V"); //Vypíše napětí na baterii

  /*----------- SHT40 -----------*/
  if (! sht4.begin()) 
  {
    Serial.println("SHT4x not found");
    Serial.println("Check connection");
    while (1) delay(1);
  }

  sht4.setPrecision(SHT4X_HIGH_PRECISION); // the higest resolution
  sht4.setHeater(SHT4X_NO_HEATER); // no heater

  sensors_event_t humidity, temperature; // variable for humidity and temperature
  sht4.getEvent(&humidity, &temperature); // read value

  Serial.print("Temperature: "); Serial.print(temperature.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  display.init(115200);

  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);  // white background
  display.setTextColor(GxEPD_BLACK);  // black font

  display.setFullWindow();
  display.firstPage();
  do {
    display.setCursor(/*x*/5, /*y*/50); // set cursor
    display.setFont(&OpenSansSB_50px); // font
    display.print(String(temperature.temperature, 1)); 
    display.print("  ");
    display.print(String(humidity.relative_humidity, 0)); 

    display.setFont(&OpenSansSB_12px); // font
    display.setCursor(/*x*/30, /*y*/70); // set cursor
    display.println(" degC");
    display.setCursor(/*x*/150, /*y*/70); // set cursor
    display.println(" % Rh");

    display.setFont(&OpenSansSB_50px); // font
    display.setCursor(/*x*/5, /*y*/150); // set cursor
    display.print(String(vBat, 1)); 


    display.setFont(&OpenSansSB_12px); // font
    display.setCursor(/*x*/5, /*y*/170); // set cursor
    display.print("Battery voltage, V");
  }
  while (display.nextPage());

  delay(100);
  digitalWrite(POWER, LOW); // disable power supply for ePaper

  // deep sleep mode
  esp_sleep_enable_timer_wakeup(300 * 1000000);
  delay(200);
  esp_deep_sleep_start();
}

void loop() {

}
