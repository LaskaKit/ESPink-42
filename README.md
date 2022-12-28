![ESPink-4.2" top](https://github.com/LaskaKit/ESPink-42/blob/main/img/ESPink-42-2.jpg)

# ESPink 4.2" - versatile and low power board with ESP32 for big 4.2" ePaper/eInk

[ePaper displays](https://www.laskakit.cz/e-ink/) are more and more popular mainly thanks to low power consumption and wide viewing angle. 
But what board you should use to control it? We designed ESPink, small board focused on the lowest power consumption, easy connection of sensors and big memory for graphics. 
The ESPink is based on ESP32 what is one of the most popular Wi-Fi and Bluetooth modules. 

What is on board? Connector for connection of ePaper/eInk. The ePaper is very low power but it is drawing current during sleep (a few uA). To achieve the lowest current, we assembled transistor between power and input of ePaper. Thanks to this, the current through ePaper during the sleep is zero. The gate of transistor is connected to GPIO2 and the power supply for ePaper is enabled in case GPIO2 is set to HIGH.

One more feature how to decrease the current is the external [LaskaKit CH9102 programmer](https://www.laskakit.cz/laskakit-ch9102-programmer-usb-c--microusb--uart/),
what we are using for all of our boards. So only one programmer may upload code to all of our low power boards. If you prefer to have programmer on board, you can buy [LaskaKit CH9102 SMD programmer](https://www.laskakit.cz/laskakit-ch9102-smd-programmer/) and assemble it on board. The own power consumption will be a little bit higher.

The board is powered from [lipol battery](https://www.laskakit.cz/baterie-a-akumulatory/) what is chargered from USB-C, no longer micro USB.
The design is ready for measurement of battery voltage. The voltage divider is connected to battery and GPIO34 where is ADC input (solder bridge ADC is by defoult open, you shall solder it if you want to measure that).

The connection of sensors is so easy and safe thanks to our μŠup connector. The connector includes and lock and the pinout is the same for all sensors and board made by laskakit.cz.  
The sensors what include μŠup connector are [LaskaKit SHT40 Senzor of temperature and humidity](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/) and  [LaskaKit SCD41 Senzor CO2, temperature and humidity ](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/).

ESP32 includes a lot of GPIO what we didn't use, but we keep them for custom purpose. So you can solder what you want.

The charging current of on-board lipol charger is set to 400mA.

ESPink 4.2" is available on https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/
We wrte an example codes available on this link https://github.com/LaskaKit/ESPink-42/tree/main/SW

![ESPink-4.2" pinout](https://github.com/LaskaKit/ESPink-42/blob/main/img/ESPink-4.2-pinout.jpg)
