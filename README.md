![ESPink 4.2" top](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-espink-42-esp32-e-paper-pcb-antenna-2.jpg)
![ESPink 4.2" bottom](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-espink-42-esp32-e-paper-pcb-antenna-3.jpg)

# LaskaKit ESPink-4.2" — universal and ultra-low-power ESP32 board for 4.2" ePaper displays

[ePaper displays](https://www.laskakit.cz/e-ink/) are becoming incredibly popular — and for good reason.  
Their extremely low power consumption and wide viewing angle make them ideal for many battery-powered projects.  
But what’s the best way to drive such a display? That’s why we designed **ESPink** — a board with large memory, low power consumption, easy I²C sensor connection, and a huge maker community around ESP32.

---

## Main Features

The board naturally includes a connector for ePaper displays (sometimes referred to as *E-Ink*).  
While ePaper is very energy-efficient, it still draws a few microamps even when idle.  
To minimize this current, display power is controlled by a voltage regulator switched through **GPIO47**.  
The display power turns on when GPIO47 is set to HIGH.

The board can be powered by a **Li-Po battery**, which is charged via the **USB-C** connector.  
Battery voltage can be measured thanks to an integrated voltage divider connected to **GPIO9**.

Connecting sensors has never been easier or safer.  
No more broken sensors due to reversed polarity — with our **μŠup connector** (with a locking mechanism), you can plug sensors in quickly and securely.  
Compatible sensors include:  
- [LaskaKit SHT40 temperature and humidity sensor](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/)  
- [LaskaKit SCD41 CO₂, temperature, and humidity sensor](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/)

The board also provides plenty of **GPIO pins** for your own extensions or modules.  
Power consumption in deep-sleep mode is extremely low — perfect for long-term battery projects.

---

## Important Notice

Recently shipped boards are equipped with **GoodDisplay displays**.  
The original **Waveshare** versions are no longer in production.  
At this time, there may be **incompatibility with ESPHome**, since support for new GoodDisplay panels is not yet fully implemented.  
If you plan to use ESPHome, please verify compatibility before purchase.

---

## Versions and Specifications

### ESPink-4.2" v3.0

| Display Type | Model | Colors | Refresh |
|---------------|------------------|--------------------------|----------------|
| Black & White | GDEQ042T81 | BW Grayscale | partial refresh |
| Black & White (touch + backlight) | GDEY042T81-FT02 | BW Touch | partial refresh |
| Red-Black-White | GDEY042Z98 | RBW | full refresh ~22 s |
| Yellow-Red-Black-White | GDEM042F52 | YRBW | full refresh ~20 s |

**ESP32 module:** ESP32-S3-N16R8 (16 MB Flash, 8 MB PSRAM)  
**Input voltage:** 3.7 – 5.5 V  
**Power consumption (deep-sleep):**
- Without touch: 16 µA  
- With touch: 4.5 mA for first 30 s, then 1.55 mA  
**Digital I/O pins:** 32  
**Analog inputs:** 16  
**Recommended battery:** GeB Li-Po 503759 – 1200 mAh  

![ESPink 4.2" case](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-krabicka-pro-espink-42-3.jpg)

---

## Pinout (GxEPD2 standard)

| Function | Pin |
|-----------|-----|
| MOSI / SDI | 23 |
| CLK / SCK | 18 |
| SS / CS | 5 |
| DC | 17 |
| RST | 16 |
| BUSY | 4 |
| ePaper Power (HIGH = ON) | 47 |

---

## μŠup Connector Pinout

### SPI_uSUP
| Signal | Pin |
|---------|-----|
| CS | 46 |
| MOSI | 3 |
| SCK | 14 |
| MISO | 21 |

### I2C_uSUP
| Signal | Pin |
|---------|-----|
| SDA | 42 |
| SCL | 2 |

### SD Card
| Signal | Pin |
|---------|-----|
| CS | 17 |
| MOSI | 11 |
| SCK | 12 |
| MISO | 13 |
| SD Insert | 16 |

### Buttons
| Function | Pins |
|-----------|------|
| Buttons | 39, 40, 41 |

---

📦 **Buy ESPink-4.2" in our e-shop:**  
➡️ [https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/](https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/)

💾 **Example codes and demos:**  
➡️ [https://github.com/LaskaKit/ESPink-42/tree/main/SW](https://github.com/LaskaKit/ESPink-42/tree/main/SW)