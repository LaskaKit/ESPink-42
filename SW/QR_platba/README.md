# QR Platba na [ESPink4.2](https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/?variantId=13538) (ESP32 + e‑paper)

Projekt vytváří platební QR kód ve formátu Short Payment Descriptor (QR Platba, SPD 1.0) přímo na desce **[LaskaKit ESPink4.2](https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/?variantId=13538)** s ESP32 a 4,2" e‑paper displejem. Uživatel vyplní platební údaje přes webový formulář a výsledný QR kód se zobrazí na displeji, odkud jej zákazník naskenuje v mobilním bankovnictví.

## Funkce

- Připojení k uložené Wi‑Fi nebo vytvoření vlastní Wi‑Fi AP **„Laskakit‑Platba_QR“** (heslo **„esp32qrpay“**) s jednoduchým webovým rozhraním.
- Webový formulář pro zadání částky (Kč), IBAN účtu, variabilního symbolu a data splatnosti a následné vygenerování QR platby.
- Generování platebního řetězce ve formátu SPD 1.0 a jeho zobrazení jako QR kódu na 4,2" e‑paper displeji.
- Uložení českého IBAN a Wi‑Fi přihlašovacích údajů do EEPROM a jejich opakované použití.
- Převod českého IBAN na domácí formát účtu (předčíslí‑číslo/kód banky) pro čitelný výpis na displeji.
- Administrační stránka `/admin` pro nastavení Wi‑Fi (SSID + heslo) a restart zařízení po uložení.

## Použitý hardware

Projekt je navržen pro:

- **LaskaKit ESPink4.2** – ESP32 modul s integrovaným 4,2" e‑paper displejem.
- ESP32 (WROOM) s Wi‑Fi.
- 4,2" e‑paper připojený přes SPI:
  - `PIN_SS` = 5  
  - `PIN_DC` = 17  
  - `PIN_RST` = 16  
  - `PIN_BUSY` = 4
- Řízení napájení e‑paper displeje na pinu `DISPLAY_POWER_PIN` (GPIO2).

Ostatní ESP32 piny jsou dle standardního zapojení desky ESPink4.2.

## Závislosti (knihovny)

Projekt je určen pro **Arduino IDE** (nebo kompatibilní build systém) s podporou ESP32.

Povinné knihovny:

- **WiFi.h** – Wi‑Fi připojení ESP32.
- **ESPAsyncWebServer** – asynchronní webový server (HTTP GET/POST, formuláře).
- **EEPROM.h** – ukládání IBAN a Wi‑Fi údajů.
- **GxEPD2_BW** – ovladač pro 4,2" e‑paper displej (černobílý režim).
- **qrcode_gen.h** (Ricmoo QRCode) – generování QR kódu z textového řetězce.
- **FreeMonoBold9pt7b** – font pro zobrazení textu na displeji.

Před kompilací ověř, že jsou knihovny nainstalované přes Library Manager nebo přidány jako Git submoduly.

## Kompilace

V Arduino IDE:

1. Nainstaluj podporu ESP32 (Boards Manager).
2. Otevři projektový `.ino` nebo `.cpp` soubor.
3. V **Tools → Board** vyber **ESP32 Dev Module**.
4. Nastav správný port a klikni na **Upload**.

## Jak to funguje

1. ESP32 po startu načte z EEPROM uložené Wi‑Fi přihlašovací údaje.
2. Pokusí se připojit k Wi‑Fi (režim STA) po dobu cca 20 s.
3. Pokud se nepřipojí, vytvoří vlastní AP **„Laskakit‑Platba_QR“** s heslem **„esp32qrpay“**.
4. Na e‑paper displeji zobrazí stav Wi‑Fi (připojeno/AP režim), SSID a IP adresu.
5. Na kořenové URL `/` běží stránka s formulářem:
   - Částka v Kč.
   - IBAN (český, formát CZ…).
   - Variabilní symbol (volitelné).
   - Datum splatnosti (volitelné).
6. Po odeslání na `/qr` proběhne:
   - Očištění IBAN od mezer a nealfanumerických znaků.
   - Kontrola, že IBAN začíná `CZ` a má délku 24 znaků.
   - Kontrola částky > 0.
   - Validace VS (max. 10 číslic).
   - Uložení IBAN do EEPROM (pokud se změnil).
   - Vytvoření SPD řetězce a vygenerování QR kódu.
   - Vykreslení QR kódu a textových údajů (částka, účet, VS) na e‑paper displej.
7. Admin stránka `/admin` umožní změnu Wi‑Fi SSID/hesla a restart zařízení.

## End‑pointy

- `GET /` – hlavní stránka pro zadání platebních údajů a vytvoření QR kódu.
- `GET /qr` – zpracování formuláře, validace vstupů, generování QR platby a vykreslení na e‑paper.
- `GET /admin` – formulář pro nastavení Wi‑Fi (SSID a heslo).
- `POST /wifi_save` – uložení Wi‑Fi údajů do EEPROM a restart ESP32.

## Bezpečnost a omezení

- Ukládání údajů do EEPROM není šifrované, projekt je tedy vhodný pro jednoduché pokladní/klubové použití, ne pro kritické scénáře.
- Validace je primárně uzpůsobena českému IBAN (`CZ` + 24 znaků).
- Okamžité platby jsou indikované parametrem `PT:IP` pouze na úrovni SPD řetězce – skutečné provedení závisí na bance.

## Možné vylepšení

- Přidání zprávy pro příjemce a dalších volitelných SPD parametrů.
- Volitelné předvyplnění částky a VS (např. přes query parametry v URL).
- Podpora více účtů nebo profilů.
- Jednoduchá ochrana admin rozhraní (heslo, PIN).

## Hardware

ESPink42 můžete zakoupit na https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/?variantId=13538
