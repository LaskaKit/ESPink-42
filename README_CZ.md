![ESPink 4.2" top](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-espink-42-esp32-e-paper-pcb-antenna-2.jpg)
![ESPink 4.2" bottom](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-espink-42-esp32-e-paper-pcb-antenna-3.jpg)

# LaskaKit ESPink-4.2" — univerzální a velmi úsporná deska s ESP32 pro 4.2" ePaper

[ePaper displeje](https://www.laskakit.cz/e-ink/) nabírají neuvěřitelné popularity a není divu.  
Především velmi nízký odběr a široký pozorovací úhel dělá ePaper tak populárním.  
Ale čím tento displej řídit? Proto jsme navrhli **ESPink** — desku s velkou pamětí, nízkým odběrem, snadným připojením I2C čidel a velkou komunitou bastlířů okolo ESP32.

---

## Hlavní vlastnosti

Na desce samozřejmě najdete konektor pro ePaper displeje (někdy označovaný jako *E-Ink*).  
Displej sám o sobě je velmi úsporný, ale i mimo fázi přepisu odebírá několik mikroampér.  
Abychom tento odběr minimalizovali, je napájení displeje řízené stabilizátorem spínaným přes **GPIO47**.  
Napájení ePaperu se tedy zapne, když je GPIO47 nastaven do HIGH.

Deska může být napájena z **LiPol akumulátoru**, který se nabíjí přes **USB-C** konektor.  
Napětí akumulátoru lze měřit díky integrovanému děliči připojenému na **GPIO9**.

Připojení čidel nebylo nikdy snadnější ani bezpečnější.  
Žádná nefunkční čidla kvůli přepólování — díky našemu **μŠup konektoru**, který obsahuje zámek, čidlo jednoduše a bezpečně připojíte.  
Mezi taková čidla patří například:  
- [LaskaKit SHT40 senzor teploty a vlhkosti vzduchu](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/)  
- [LaskaKit SCD41 senzor CO₂, teploty a vlhkosti vzduchu](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/)

Na desce je také dostatek **GPIO pinů** pro rozšiřující moduly nebo vlastní projekty.  
Odběr v hlubokém spánku (deep-sleep) je velmi nízký — ideální pro bateriové projekty.

---

## Důležité upozornění

Nově prodávané desky využívají **displeje od výrobce GoodDisplay**.  
Původní displeje od **Waveshare** se již nevyrábějí.  
V současné době může být **nekompatibilita s ESPHome**, protože zatím není implementována podpora pro nové displeje GoodDisplay.  
Pokud používáte ESPHome, doporučujeme tuto informaci ověřit před nákupem.

---

## Verze a parametry

### ESPink-4.2" v3.0

| Typ displeje | Model | Barvy | Obnova |
|---------------|------------------|--------------------------|----------------|
| Černo-Bílý | GDEQ042T81 | BW Grayscale | částečný refresh |
| Černo-Bílý (dotyk + podsvit) | GDEY042T81-FT02 | BW Touch | částečný refresh |
| Červeno-Černo-Bílý | GDEY042Z98 | RBW | full refresh ~22 s |
| Žluto-Červeno-Černo-Bílý | GDEM042F52 | YRBW | full refresh ~20 s |

**ESP32 modul:** ESP32-S3-N16R8 (16 MB Flash, 8 MB PSRAM)  
**Napájení:** 3.7 – 5.5 V  
**Odběr (deep-sleep):**
- Bez dotyku: 16 µA  
- S dotykem: 4.5 mA prvních 30 s, poté 1.55 mA  
**Digitální I/O piny:** 32  
**Analogové vstupy:** 16  
**Vhodná baterie:** GeB LiPol 503759 – 1200 mAh  

![ESPink 4.2" bottom](https://github.com/LaskaKit/ESPink-42/blob/main/img/LaskaKit-krabicka-pro-espink-42-3.jpg)


---

## Pinout (GxEPD2 standard)

| Funkce | Pin |
|--------|-----|
| MOSI / SDI | 23 |
| CLK / SCK | 18 |
| SS / CS | 5 |
| DC | 17 |
| RST | 16 |
| BUSY | 4 |
| Napájení ePaperu (HIGH = ON) | 47 |

---

## Pinout konektorů μŠup

### SPI_uSUP
| Signál | Pin |
|---------|-----|
| CS | 46 |
| MOSI | 3 |
| SCK | 14 |
| MISO | 21 |

### I2C_uSUP
| Signál | Pin |
|---------|-----|
| SDA | 42 |
| SCL | 2 |

### SD karta
| Signál | Pin |
|---------|-----|
| CS | 17 |
| MOSI | 11 |
| SCK | 12 |
| MISO | 13 |
| SD Insert | 16 |

### Tlačítka
| Funkce | Piny |
|---------|------|
| Buttons | 39, 40, 41 |

---

📦 **ESPink-4.2" na e-shopu:**  
➡️ [https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/](https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/)

💾 **Vzorové kódy a ukázky:**  
➡️ [https://github.com/LaskaKit/ESPink-42/tree/main/SW](https://github.com/LaskaKit/ESPink-42/tree/main/SW)