/*
* Example code shows values from weatherstation from TMEP.cz (temperature, humidity, pressure, battery voltage) 
* and BME280 (temperature/humidity), battery voltage on eInk/ePaper display
*
* Compile as ESP32 dev module
* ESPink - ESP32 board for ePaper: https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
* BME280 (https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/) is connected through uSup connector 
***********************************************
* Based on your settings, you have to change:
* WiFi setting - SSID and Password
* TMEP server - jsonurl
***********************************************
* Libraries:
* EPD library: https://github.com/ZinggJM/GxEPD2
* ArduinoJson: https://arduinojson.org/
* NTPClient: https://github.com/arduino-libraries/NTPClient
* Font: https://oleddisplay.squix.ch/ 
* BME280: https://github.com/adafruit/Adafruit_BME280_Library
* https://github.com/adafruit/Adafruit_Sensor
***********************************************
* made by laskakit (c) 2023
*/
 
#include <WiFi.h>

// ePaper
#include <GxEPD2_BW.h>
#include "SPI.h"

// NTPClient
#include "time.h"
#include "sntp.h"

// ArduinoJson
#include <ArduinoJson.h>

// BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
 
// Fonts
#include "OpenSansSB_12px.h"
#include "OpenSansSB_24px.h"
#include "OpenSansSB_50px.h"

      /***********************/
      /* PLEASE, CHANGE THIS */
      /***********************/
/* -----------------WiFi network ---------------- */
char ssid[] = "xxx";
char pass[] = "xxx";
/* ---------------------------------------------- */

/* -------------------TMEP setting--------------- */
const char* host = "tmep.cz"; // TMEP.cz or TMEP.eu
const int httpPort = 80;
//You can try with our test Weather Station first 
//const char* jsonurl = "https://tmep.cz/vystup-json.php?id=4551&export_key=bx9hd5jq8g&extended=1"; //LasKKit Test Weather Station, Rychnov nad Kneznou
const char *jsonurl = "https://tmep.cz/vystup-json.php?id=6707&export_key=68t2vd093s&extended=1";  // LaskaKit Maker Faire Weather Station
/* ---------------------------------------------- */

/* ---------- Deepsleep time in seconds --------- */
#define deepSleepTime 300
/* ---------------------------------------------- */

/* -------------------Board--------------- */
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

/*  ------------------------------------------------------------------------------------------------- */

/*-------------- ePaper resolution----------------*/
#define DISPLAY_RESOLUTION_X 400
#define DISPLAY_RESOLUTION_Y 300
/* ---------------------------------------------- */

/* -------------------NTP Client----------------- */
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
// A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3"; // Prague time zone
/* ---------------------------------------------- */

/* ---- ADC reading - indoor Battery voltage ---- */
#define dividerRatio 1.7693877551  // Voltage devider ratio on ADC pin 1MOhm + 1.3MOhm
/* ---------------------------------------------- */

/* -------------------SHT40--------------------- */
#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
/* ---------------------------------------------- */

/* variables */
float temperature_out;  // temperature from outdoor weatherstation (from tmep.cz)
int pressure_out; // pressure from outdoor weatherstation (from tmep.cz)
int humidity_out; // humidity from outdoor weatherstation (from tmep.cz)
float m_volt; // battery voltage from outdoor weatherstation (from tmep.cz)

String date; // actual time

int strength; // Wi-Fi signal strength

int percentage; // indoor battery voltage in percent
float d_volt; // indoor battery voltage
float temperature_in; // indoor temperature from SHT40
int humidity_in; // indoor humidity from SHT40

/* functions */
void readTMEPdata() {
  // Connect to the HOST and read data via GET method
  WiFiClient client; // Use WiFiClient class to create TCP connections
 
  Serial.print("Connecting to "); Serial.println(host);
  if (!client.connect(host, httpPort)) {
    // If you didn't get a connection to the server
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Client connected");
 
  // Make an url
  char url[70];                                   // Make new array, 50 bytes long
  strncpy(url, &jsonurl[15], strlen(jsonurl)-14); // Remove "https://tmep.cz" and leave rest
 
  Serial.print("Requesting URL: "); Serial.println(url);
 
  // Make HTTP GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
 
  // Workaroud for timeout
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  // Read JSON
  String data;
  bool capture = false;
  String json;
  while (client.available()) {
    data = client.readStringUntil('\n'); // Loop over rows. "\r" not work. "\0" returns all at one
    //Serial.println(data);
    // First few rows is header, it ends with empty line.
    // This is unique ultrasimple, but working solution.
    // Start capture when { occurs and end capturing with }.
    if(data.startsWith("{", 0)) { // JSON starts with {. Start capturing.
      capture = true;
    }
    if(data.startsWith("}", 0)) { // JSON ends with }. Stop capturing.
      capture = false;
    }
    if(capture) {
      json = json + data; // Joining row by row together in one nice JSON part.
    }
  }
  json = json + "}"; // WTF, last bracket is missing :/ add it here - ugly!
  Serial.println(json);
 
  // Lets throw our json on ArduinoJson library and get results!
  StaticJsonDocument<1000> doc; 
  deserializeJson(doc, json);
  auto error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }
  // This is how our JSON looks. You can read any value of parameters
  // {
  //  "teplota": 24.87,
  //  "vlhkost": null,
  //  "cas": "2016-06-02 18:29:45",
  //  "umisteni": "Trutnov"
  // }
 
  //---------------- Temperature ----------------//
  Serial.println("Outdoor weatherstation");
  temperature_out = doc["teplota"]; // Get value of "teplota"
  Serial.println("Temperature: " + String(temperature_out));
  //---------------- Pressure ----------------//
  pressure_out = doc["tlak"];
  Serial.println("Pressure: " + String(pressure_out));
   //---------------- Humidity ----------------//
  humidity_out = doc["vlhkost"];
  Serial.println("Humidity: " + String(humidity_out));
  //---------------- Battery voltage ----------------//
  m_volt = doc["napeti"];
  m_volt = round(m_volt*100.0)/100.0;     // round to x,xx
  Serial.println("Meteo Battery voltage: " + String(m_volt));
  //---------------- Date ----------------//
  date = doc["cas"].as<String>();
  Serial.println("Update date: " + date);
}

int8_t getWifiStrength() {
  strength = WiFi.RSSI();
  Serial.println("Wifi Strength: " + String(strength) + " dB"); 

  return strength;
}

uint8_t getVBattery() {
  // battery voltage measurement
  d_volt = analogReadMilliVolts(BAT) * dividerRatio/1000;
  Serial.println("Battery voltage: " + String(d_volt) + " V");

  return d_volt;
}

void WiFiInit(){
  // Connecting to WiFi
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    i++;
    if (i == 10) {
      i = 0;
      Serial.println("Not able to connect");
      digitalWrite(POWER, LOW); // disable power supply for ePaper
      // deep sleep mode
      esp_sleep_enable_timer_wakeup(deepSleepTime * 1000000);
      delay(200);
      esp_deep_sleep_start();
    } else {
      Serial.print("."); 
    }
  }
  Serial.println("");
  Serial.println("Wi-Fi connected successfully");
}

void displayInit(){
  display.init();

  display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);  // white background
  display.setTextColor(GxEPD_BLACK);  // black font
}

void SHT40init(){
  if (! sht4.begin()) {
    Serial.println("SHT4x not found");
    Serial.println("Check connection");
    while (1) delay(1);
  }
  sht4.setPrecision(SHT4X_HIGH_PRECISION); // the higest resolution
  sht4.setHeater(SHT4X_NO_HEATER); // no heater
}

void getSHT40data(){
  Serial.println("Indoor weatherstation");

  sensors_event_t humidity, temperature; // variable for humidity and temperature
  sht4.getEvent(&humidity, &temperature); // read value

  temperature_in = temperature.temperature;
  Serial.print("Temperature = ");
  Serial.print(temperature_in);
  Serial.println(" °C");

  humidity_in = humidity.relative_humidity;
  Serial.print("Humidity = ");
  Serial.print(humidity_in);
  Serial.println(" %");
}

void printTime() {
  char buff[6];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    return;
  }
  sprintf(buff, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  display.print(buff);
}

void setup() {
  Serial.begin(115200);
  pinMode(POWER, OUTPUT); 
  digitalWrite(POWER, HIGH); // enable power supply for ePaper
  delay(500);
  Wire.begin (SDA, SCL);
  // ePaper init
  displayInit();
  // Wifi init
  WiFiInit();
  //SHT40 init
  SHT40init();
  // Time config
  configTzTime(time_zone, ntpServer1, ntpServer2);
  // get Time from NTPClient
  display.setTextColor(GxEPD_BLACK, GxEPD_WHITE);
  display.setFont(&OpenSansSB_50px);
  display.setCursor(DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/5); // set cursor
  printTime();

  // read sensor data from TMEP
  readTMEPdata();
  // WiFi strength
  getWifiStrength();
  // Battery votlage IN
  getVBattery();
  // get data from indoor SHT40
  getSHT40data();


  //--------------------------v
  //   X   |        |        |
  //--------------------------
  //       |        |        |
  //--------------------------
  display.fillRect(0, DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect(0+3, DISPLAY_RESOLUTION_Y/3+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(10, DISPLAY_RESOLUTION_Y/3+15);
  display.print("Temp OUT [degC]");
  display.setFont(&OpenSansSB_50px);
  display.setCursor(10, DISPLAY_RESOLUTION_Y/3+70);
  display.print(String(temperature_out, 1));
  //--------------------------
  //       |   X    |        |
  //--------------------------
  //       |        |        |
  //--------------------------
  display.fillRect(DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect(DISPLAY_RESOLUTION_X/3+3, DISPLAY_RESOLUTION_Y/3+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(150, DISPLAY_RESOLUTION_Y/3+15);
  display.print("Press OUT [hPa]");
  display.setFont(&OpenSansSB_50px);
  display.setCursor(140, DISPLAY_RESOLUTION_Y/3+70);
  display.print(pressure_out);
  //--------------------------
  //       |        |    X   |
  //--------------------------
  //       |        |        |
  //--------------------------
  display.fillRect(2*DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect((2*DISPLAY_RESOLUTION_X/3)+3, DISPLAY_RESOLUTION_Y/3+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(280, DISPLAY_RESOLUTION_Y/3+15);
  display.print("Temp IN [degC]");
  display.setFont(&OpenSansSB_50px);
  display.setCursor(280, DISPLAY_RESOLUTION_Y/3+70);
  display.print(temperature_in);
  //--------------------------
  //       |        |        |
  //--------------------------
  //   X   |        |        |
  //--------------------------
  display.fillRect(0, 2*DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect(0+3, (2*DISPLAY_RESOLUTION_Y/3)+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(10, (2*DISPLAY_RESOLUTION_Y/3)+15);
  display.print("Hum OUT [% Rh]");
  display.setFont(&OpenSansSB_50px);
  display.setCursor(30, (2*DISPLAY_RESOLUTION_Y/3)+70);
  display.print(humidity_out);
  //--------------------------
  //       |        |        |
  //--------------------------
  //       |   X    |        |
  //--------------------------
  display.fillRect(DISPLAY_RESOLUTION_X/3, 2*DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect(DISPLAY_RESOLUTION_X/3+3, (2*DISPLAY_RESOLUTION_Y/3)+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(180, (2*DISPLAY_RESOLUTION_Y/3)+15);
  display.print("Info");
  display.setCursor(140, (2*DISPLAY_RESOLUTION_Y/3)+40);
  display.setFont(&OpenSansSB_12px);
  display.print("vBat: ");
  display.setFont(&OpenSansSB_24px);
  display.print(m_volt);
  display.print(" V");
  display.setCursor(140, (2*DISPLAY_RESOLUTION_Y/3)+65);
  display.setFont(&OpenSansSB_12px);
  display.print("Sig: ");
  display.setFont(&OpenSansSB_24px);
  display.print(strength);
  display.print(" dB");
  display.setCursor(140, (2*DISPLAY_RESOLUTION_Y/3)+90);
  display.setFont(&OpenSansSB_12px);
  display.print("vBat: ");
  display.setFont(&OpenSansSB_24px);
  display.print(d_volt);
  display.print(" V");
  //--------------------------
  //       |        |        |
  //--------------------------
  //       |        |    X   |
  //--------------------------
  display.fillRect(2*DISPLAY_RESOLUTION_X/3, 2*DISPLAY_RESOLUTION_Y/3, DISPLAY_RESOLUTION_X/3, DISPLAY_RESOLUTION_Y/3, GxEPD_BLACK);
  display.fillRect((2*DISPLAY_RESOLUTION_X/3)+3, (2*DISPLAY_RESOLUTION_Y/3)+3, DISPLAY_RESOLUTION_X/3-6, DISPLAY_RESOLUTION_Y/3-6, GxEPD_WHITE);
  display.setFont(&OpenSansSB_12px);
  display.setCursor(280, (2*DISPLAY_RESOLUTION_Y/3)+15);
  display.print("Hum IN [% Rh]");
  display.setFont(&OpenSansSB_50px);
  display.setCursor(300, (2*DISPLAY_RESOLUTION_Y/3)+70);
  display.print(humidity_in);

  display.display(false); // update 
  delay(100);
  digitalWrite(POWER, LOW); // disable power supply for ePaper

  // deep sleep mode
  esp_sleep_enable_timer_wakeup(deepSleepTime * 1000000);
  delay(200);
  esp_deep_sleep_start();
}

void loop() {

}
