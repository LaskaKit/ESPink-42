![ESPink 4.2" top](https://github.com/LaskaKit/ESPink-42/blob/main/img/ESPink-42-2.jpg)

# ESPink-4.2" - univerzální a velmi úsporná deska s ESP32 pro velký 4.2" ePaper

[ePaper displeje](https://www.laskakit.cz/e-ink/) nabírají neuřitelné popularity a není divu. Především velmi nízký odběr a široký pozorovací úhel dělá ePaper tak populárním. 
Ale čím tento displej řídit? Proto jsme navrhli ESPink-4.2". Velká paměť, nízký odběr, snadné připojení I2C čidel a velká komunita bastlířů okolo ESP32.

Na desce samozřejmě najdeš konektor pro ePaper displeje. Někdy nazvaný také Eink. Displej sám o sobě je velmi úsporný, ale stejně i mimo fázi přepisu odebírá nějaké ty uA. 
I takovému odběru jsme chtěli zamezit a tak jsme do napájení přidali tranzistor, který napájení spíná. Tento tranzistor je připojen na GPIO2 a napájení ePaperu je připojeno, 
pokud se GPIO2 nastaví do HIGH.

Nízká spotřeba je dosažena i tím, že jsme programátor přesunuli na náš univerzální [LaskaKit CH9102 programmer]( LaskaKit CH9102 Programmer USB-C, microUSB, UART ),
který používáme u všech našich desek.

Deska může být napájena z [lipol akumulátoru](https://www.laskakit.cz/baterie-a-akumulatory/), který je nabíjen z USB-C konektoru. 
Samozřejmě můžeš měřit i napětí na akumulátoru díky integrovanému děliči napětí a přivedeného na GPIO34 (nezapomeň propájet pájecí most).

Připojení čidel nebylo nikdy snažší a bezpečnější. Žádná nefungující čidla způsobená přepolováním. Díky našemu μŠup konektoru, který obsahuje i zámek čidla jednoduše připojíš. 
Mezi taková čidla s μŠup konektorem patří [LaskaKit SHT40 Senzor teploty a vlhkosti vzduchu ](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/) nebo  [LaskaKit SCD41 Senzor CO2, teploty a vlhkosti vzduchu ](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/).

K dispozici je i spousta GPIO, na které si můžeš zapájet, co budeš potřebovat. Nabíjecí proud Lipol akumulátoru je nastaven na 400mA. 

ESPink-4.2" najdeš na našem e-shopu https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/
A několik vzorových kódu i na našem githubu https://github.com/LaskaKit/ESPink-42/tree/main/SW
