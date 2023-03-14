Example code shows values from weatherstation from TMEP.cz (temperature, humidity, pressure, battery voltage) 
and BME280 (temperature/humidity), battery voltage on eInk/ePaper display

Compile as ESP32 dev module
ESPink - ESP32 board for ePaper: https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
BME280 (https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/) is connected through uSup connector 

ESPink pinout:
MOSI/SDI 23
CLK/SCK 18
SS 5 //CS
DC 17 
RST 16  
BUSY 4 
ePaperPowerPin  2

I2C pinout:
SDA 21
SCL 22

Need to change based on your settings:
WiFi (SSID and Password)
TMEP server (jsonurl)

Libraries:
Analog read with calibration data: https://github.com/madhephaestus/ESP32AnalogRead/ 
EPD library: https://github.com/ZinggJM/GxEPD2
ArduinoJson: https://arduinojson.org/
NTPClient: https://github.com/arduino-libraries/NTPClient
Font: https://oleddisplay.squix.ch/ 
BME280: https://github.com/adafruit/Adafruit_BME280_Library
https://github.com/adafruit/Adafruit_Sensor

made by laskakit (c) 2023
