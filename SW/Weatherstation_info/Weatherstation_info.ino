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
char pass[] = "yyy";
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

      /***********************/
      /**** KEEP AS IT IS ****/
      /***********************/
/*----------------- Pinout of ESPink -------------*/
// MOSI/SDI 23
// CLK/SCK 18
// CS 5
#define SS 5 //SS
#define DC 17 // D/C
#define RST 16  // RES
#define BUSY 4  //BUSY
#define ePaperPowerPin  2
/* ---------------------------------------------- */

/*----------------- Pinout of I2C ----------------*/
#define SDA 21
#define SCL 22
/* ---------------------------------------------- */

/*------------------------  Define EPD driver - uncomment the used one  -----------------------------*/
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 1.54" b/w
//GxEPD2_BW<GxEPD2_154_M10, GxEPD2_154_M10::HEIGHT> display(GxEPD2_154_M10(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 1.54" b/w DES
//GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 2.13" b/w
//GxEPD2_BW<GxEPD2_213_M21, GxEPD2_213_M21::HEIGHT> display(GxEPD2_213_M21(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 2.13" b/w DES
//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/DC, /*RST=*/RST, /*BUSY=*/BUSY)); // GDEW042T2 400x300, UC8176 (IL0398)
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); //GDEY042T81, 400x300, SSD1683 (no inking)
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> display(GxEPD2_420c_Z21(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEQ042Z21 400x300, UC8276
//GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS*/ SS, /*DC*/ DC, /*RST*/ RST, /*BUSY*/ BUSY)); // 7.5" b/w 800x480
// Note: all supported ePapers you can find on https://github.com/ZinggJM/GxEPD2
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
#define vBatPin 34
/* ---------------------------------------------- */

/* -------------------BME280--------------------- */
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25)
#define BME280_ADDRESS (0x77)   // (0x76) cut left and solder left pad on board
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
float temperature_in; // indoor temperature from BME280
int pressure_in; // indoor pressure from BME280
int humidity_in; // indoor humidity from BME280

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
  d_volt = analogReadMilliVolts(vBatPin) * dividerRatio;
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
      digitalWrite(ePaperPowerPin, LOW); // disable power supply for ePaper
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

void BME280init(){
  Wire.begin (SDA, SCL); // ESP32 + uŠup
  bme.begin(BME280_ADDRESS);
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
}

void getBME280data(){
  Serial.println("Indoor weatherstation");
  temperature_in = bme.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature_in);
  Serial.println(" °C");

  pressure_in = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(pressure_in);
  Serial.println(" hPa");

  humidity_in = bme.readHumidity();
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
  pinMode(ePaperPowerPin, OUTPUT); 
  digitalWrite(ePaperPowerPin, HIGH); // enable power supply for ePaper
  delay(500);

  // ePaper init
  displayInit();
  // Wifi init
  WiFiInit();
  //BME280 init
  BME280init();

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
  // get data from indoor BME280
  getBME280data();


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
  digitalWrite(ePaperPowerPin, LOW); // disable power supply for ePaper

  // deep sleep mode
  esp_sleep_enable_timer_wakeup(deepSleepTime * 1000000);
  delay(200);
  esp_deep_sleep_start();
}

void loop() {

}
