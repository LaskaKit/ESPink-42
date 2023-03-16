![ePaper TOP](https://github.com/LaskaKit/ESPink-42/blob/main/SW/Weatherstation%20info/ESPink-4.2_1.jpg)</br>
</br>
Example code shows values from weatherstation from TMEP.cz (temperature, humidity, pressure, battery voltage) 
and BME280 (temperature/humidity), battery voltage on eInk/ePaper display</br>
</br>
Compile as ESP32 dev module</br>
ESPink - ESP32 board for ePaper: https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/</br>
BME280 (https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/) is connected through uSup connector </br>
</br>
ESPink pinout:</br>
MOSI/SDI 23</br>
CLK/SCK 18</br>
SS 5 //CS</br>
DC 17 </br>
RST 16  </br>
BUSY 4 </br>
ePaperPowerPin  2</br>
</br>
I2C pinout:</br>
SDA 21</br>
SCL 22</br>
</br>
Battery input:</br>
vBatPin 34</br>
</br>
Need to change based on your settings:</br>
WiFi (SSID and Password)</br>
TMEP server (jsonurl)</br>
</br>
Libraries:</br>
Analog read with calibration data: https://github.com/madhephaestus/ESP32AnalogRead/ </br>
EPD library: https://github.com/ZinggJM/GxEPD2</br>
ArduinoJson: https://arduinojson.org/</br>
NTPClient: https://github.com/arduino-libraries/NTPClient</br>
Font: https://oleddisplay.squix.ch/ </br>
BME280: https://github.com/adafruit/Adafruit_BME280_Library</br>
https://github.com/adafruit/Adafruit_Sensor</br>
</br>
made by laskakit (c) 2023</br>
</br>
![ePaper TOP](https://github.com/LaskaKit/ESPink-42/blob/main/SW/Weatherstation%20info/ESPink-4.2_2.jpg)</br>
