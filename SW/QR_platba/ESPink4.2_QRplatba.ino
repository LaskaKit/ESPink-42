/*
* Platba QR kodem primo na ucet
*
* ESPink4.2 vytvori AP (Access Point), pokud se mu do 20s nepodari pripojit k vasi Wi-Fi.
* Pokud ESPink4.2 vytvori Acess Point, prihlaste se na vytvorenou Wi-Fi "Laskakit-Platba_QR" a zadejte heslo "esp32qrpay".
* Ve webovem prohlizeci prejdete na IP zobrazenou na ePaper (nejcasteji 192.168.4.1), kde muzete zadat udaje vasi Wi-Fi.
* Ve webovem prohlizeci muze uzivatel prejit na stranku s nastavenim Wi-Fi a pripojit se do sve site.
* ESPink v obou pripadech (at uz vytvoril Access Point nebo se pripojil na Wi-Fi):
* - vytvori web stranku, kde vyplnite castku, variabilni cislo, cislo uctu a datum splatnosti.
* - QR kod se zadanymi udaji se zobrazi na ePaper displeji a zakaznik muze kod naskenovat 
* ve svem internetovem bankovnictvi a zaplatit.
* - udaje jsou zobrazeny i na ePaper displeji.
* ESPink4.2 si pamatuje prihlasovaci udaje k Wi-Fi a cislo uctu. V pripade zmeny je ulozi do pameti a priste pouzije aktualizovane.
*
* HW: ESPink4.2 https://www.laskakit.cz/laskakit-espink-42-esp32-e-paper-pcb-antenna/?variantId=13538
*
* Kompilace: Tools -> Boards -> ESP32 Dev Module
*
* Knihovny:
* https://github.com/ricmoo/qrcode/
* https://github.com/ZinggJM/GxEPD2
* https://github.com/ESP32Async/ESPAsyncWebServer
*
* laskakit (c) 2025
*
*/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <GxEPD2_BW.h>
#include "qrcode_gen.h"
#include <Fonts/FreeMonoBold9pt7b.h>

#define DISPLAY_POWER_PIN 2
#define PIN_SS    5
#define PIN_DC    17
#define PIN_RST   16
#define PIN_BUSY  4

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(PIN_SS, PIN_DC, PIN_RST, PIN_BUSY));

#define EEPROM_SIZE 104
#define IBAN_ADDR      0
#define WIFI_SSID_ADDR 40
#define WIFI_PASS_ADDR 72
#define WIFI_CRED_SIZE 32
#define IBAN_SIZE      24

#define QR_SIZE 3
#define QR_PIXEL_SIZE 8

AsyncWebServer server(80);
const char* softAP_ssid = "Laskakit-Platba_QR";
const char* softAP_pass = "esp32qrpay";

// Odstraní z textu vše kromě alfanumerických znaků
String cleanIBAN(String iban) {
  String cleaned = "";
  for (unsigned int i = 0; i < iban.length(); i++) {
    char c = iban.charAt(i);
    if (isalnum(c)) {
      cleaned += c;
    }
  }
  return cleaned;
}

// Validuje základní formát českého IBAN (musí začínat CZ a mít délku 24 znaků)
bool isValidCzIBAN(String iban) {
  return (iban.startsWith("CZ") && iban.length() == 24);
}

// Převod IBAN na české číslo účtu (předčíslí - číslo účtu / kód banky)
String ibanToCzAccount(String iban) {
  if (!isValidCzIBAN(iban)) return "";
  String kodBanky = iban.substring(4, 8);
  String predcisli = iban.substring(8, 14);
  String cisloUctu = iban.substring(14, 24);

  while (predcisli.length() > 1 && predcisli.startsWith("0")) predcisli.remove(0,1);
  while (cisloUctu.length() > 1 && cisloUctu.startsWith("0")) cisloUctu.remove(0,1);

  if (predcisli.length() > 0 && predcisli != "0")
    return predcisli + "-" + cisloUctu + "/" + kodBanky;
  else
    return cisloUctu + "/" + kodBanky;
}

// EEPROM čtení/zápis účtu a WiFi stejný jako dříve
void saveAccountToEEPROM(String account) {
  for (uint8_t i = 0; i < IBAN_SIZE; i++)
    EEPROM.write(IBAN_ADDR + i, i < account.length() ? account[i] : 0);
  EEPROM.commit();
}

String loadAccountFromEEPROM() {
  char buf[IBAN_SIZE + 1];
  for (uint8_t i = 0; i < IBAN_SIZE; i++) {
    buf[i] = EEPROM.read(IBAN_ADDR + i);
    if (buf[i] < 32 || buf[i] > 126) buf[i] = ' ';
  }
  buf[IBAN_SIZE] = '\0';
  String s(buf); s.trim();
  if (!isValidCzIBAN(s)) return "";
  return s;
}

void saveWifiToEEPROM(String ssid, String pass) {
  for (uint8_t i = 0; i < WIFI_CRED_SIZE; ++i) {
    EEPROM.write(WIFI_SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(WIFI_PASS_ADDR + i, i < pass.length() ? pass[i] : 0);
  }
  EEPROM.commit();
}

String loadWifiSSIDFromEEPROM() {
  char buf[WIFI_CRED_SIZE + 1]; buf[WIFI_CRED_SIZE] = 0;
  for (uint8_t i = 0; i < WIFI_CRED_SIZE; ++i) buf[i] = EEPROM.read(WIFI_SSID_ADDR + i);
  String s(buf); s.trim(); return s;
}

String loadWifiPassFromEEPROM() {
  char buf[WIFI_CRED_SIZE + 1]; buf[WIFI_CRED_SIZE] = 0;
  for (uint8_t i = 0; i < WIFI_CRED_SIZE; ++i) buf[i] = EEPROM.read(WIFI_PASS_ADDR + i);
  String s(buf); s.trim(); return s;
}

String cleanSSIDorPass(String s) {
  s.trim();
  if (s.length() == 0) return "";
  for (uint8_t i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if (c < 33 || c > 126) return "";
  }
  return s;
}

// Webový formulář
String generateHtmlForm(String currIban) {
  String html = R"rawliteral(
<!DOCTYPE HTML><html lang="cs"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>QR Platba – ESP32</title>
<style>
body{background:#f7fafc;font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;color:#222;margin:0;padding:0;display:flex;justify-content:center;align-items:center;min-height:100vh;box-sizing:border-box;}
.container{background:#fff;padding:2em 2.5em 1.5em 2.5em;border-radius:16px;box-shadow:0 6px 24px rgba(0,0,0,0.12);max-width:400px;width:95%;}
h2{font-size:1.9em;margin-bottom:1em;color:#2a4365;text-align:center;}
form{display:flex;flex-direction:column;gap:1.1em;}
label{font-weight:600;display:flex;flex-direction:column;font-size:1em;color:#2c3e50;}
input[type='number'],input[type='text'],input[type='date']{margin-top:0.4em;padding:0.6em 0.8em;border-radius:9px;border:1.5px solid #a0aec0;font-size:1.1em;background:#f7fafc;transition:border-color 0.3s ease;box-sizing:border-box;width:100%;font-family:inherit;}
input[type='number']:focus,input[type='text']:focus,input[type='date']:focus{border-color:#4299e1;outline:none;background:#fff;}
input[type='submit']{margin-top:1.2em;padding:0.75em 0;border:none;border-radius:10px;background:#4299e1;color:#fff;font-size:1.25em;font-weight:700;cursor:pointer;transition:background 0.25s ease;}
input[type='submit']:hover{background:#2b6cb0;}
.footer-link{text-align:center;margin-top:1.2em;font-size:0.95em;}
.footer-link a{color:#197dcc;text-decoration:none;}
.footer-link a:hover{text-decoration:underline;}
@media(max-width:480px){.container{padding:1.8em 1.5em 1.2em 1.5em;border-radius:0;box-shadow:none;width:100%;height:100vh;justify-content:center;}body{min-height:100vh;padding:10px;}input[type='number'],input[type='text'],input[type='date'],input[type='submit']{font-size:1.05em;}}
</style>
</head><body>
<div class="container" role="main">
<h2>Zadej údaje pro QR platbu</h2>
<form action="/qr" method="get" autocomplete="off" novalidate>
<label for="amount">Částka (Kč):
<input type="number" id="amount" name="amount" step="0.01" required placeholder="Např. 350.00" aria-required="true" />
</label>
<label for="account">IBAN:
<input type="text" id="account" name="account" pattern="CZ[0-9A-Z]{22}" maxlength="34" required value=")rawliteral" + currIban + R"rawliteral(" placeholder="CZxxxxxxxxxxxxxxxxxxxxxx" aria-required="true" />
</label>
<label for="vs">Variabilní symbol:
<input type="text" id="vs" name="vs" pattern="[0-9]{1,10}" maxlength="10" placeholder="max 10 cifer" />
</label>
<label for="date">Datum splatnosti:
<input type="date" id="date" name="date" />
</label>
<input type="submit" value="Vytvořit QR" />
</form>
<div class="footer-link" role="contentinfo"><a href="/admin">Nastavení WiFi</a></div>
</div>
</body></html>
)rawliteral";
  return html;
}

// Generování admin stránky
String generateAdminForm(String ssid, String pass) {
  ssid = cleanSSIDorPass(ssid);
  pass = cleanSSIDorPass(pass);
  String html = R"rawliteral(
<!DOCTYPE HTML><html lang="cs"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Nastavení WiFi</title>
<style>
body{font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;background:#fafaff;color:#222;display:flex;align-items:center;justify-content:center;height:100vh;margin:0;padding:0;}
.formbox{background:#fff;padding:2em 2.5em 2em 2.5em;border-radius:16px;box-shadow:0 6px 24px rgba(0,0,0,0.12);max-width:360px;width:95%;}
h2{text-align:center;margin-bottom:1em;color:#2a4365;font-size:1.8em;}
input[type='text'],input[type='password']{width:100%;font-size:1.1em;padding:0.6em 0.8em;margin-bottom:1.2em;border-radius:9px;border:1.5px solid #bbc;box-sizing:border-box;font-family:inherit;transition:border-color 0.3s ease;}
input[type='text']:focus,input[type='password']:focus{outline:none;border-color:#4299e1;background:#fff;}
input[type='submit']{width:100%;padding:0.75em 0;border:none;border-radius:10px;background:#4299e1;color:#fff;font-size:1.2em;font-weight:700;cursor:pointer;transition:background 0.25s ease;}
input[type='submit']:hover{background:#2b6cb0;}
.footer-link{text-align:center;margin-top:1em;font-size:0.95em;}
.footer-link a{color:#197dcc;text-decoration:none;}
.footer-link a:hover{text-decoration:underline;}
@media(max-width:480px){.formbox{border-radius:0;box-shadow:none;height:100vh;padding:1.8em 1.5em 1.2em 1.5em;}body{padding:10px;}}
</style>
</head><body><div class="formbox" role="main"><h2>Nastavení WiFi</h2>
<form action="/wifi_save" method="post" autocomplete="off" novalidate>
SSID:<br>
<input name="ssid" type="text" maxlength="31" value=")rawliteral" + ssid + R"rawliteral(" placeholder="SSID (název sítě)" autocomplete="off"><br>
Heslo:<br>
<input name="pass" type="password" maxlength="31" value=")rawliteral" + pass + R"rawliteral(" placeholder="Heslo sítě" autocomplete="off"><br>
<input type="submit" value="Uložit & restart">
</form>
<div class="footer-link" role="contentinfo"><a href="/">Zpět na platbu</a></div></div></body></html>
)rawliteral";
  return html;
}

String generateQRPlatba(String iban, String amount, String date, String vs) {
  date.replace("-", "");
  String qr = "SPD*1.0*ACC:" + iban + "*AM:" + amount + "*CC:CZK";
  if (date.length() == 0) qr += "*PT:IP";
  else qr += "*DT:" + date;
  if (vs.length() > 0) qr += "*X-VS:" + vs;
  Serial.print("Generuji QR řetězec: "); Serial.println(qr);
  return qr;
}

void showQRCode(const String &qrtext, const String &amount, const String &vs, const String &iban) {
  display.fillScreen(GxEPD_WHITE);
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(QR_SIZE)];
  qrcode_initText(&qrcode, qrcodeData, QR_SIZE, 0, qrtext.c_str());
  int qrw = QR_PIXEL_SIZE * qrcode.size;
  int offset_x = (display.width() - qrw) / 2;
  int offset_y = 16;
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(offset_x + x * QR_PIXEL_SIZE, offset_y + y * QR_PIXEL_SIZE, QR_PIXEL_SIZE, QR_PIXEL_SIZE, GxEPD_BLACK);
      }
    }
  }
  int txt_y = offset_y + qrw + 8 + 10;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(18, txt_y);
  display.print("Castka: ");
  display.print(amount);
  display.print(" Kc");
  txt_y += 32;
  display.setCursor(18, txt_y);
  display.print("Ucet: ");
  txt_y += 28;

  String domaciUcet = ibanToCzAccount(iban);
  display.setCursor(18, txt_y);
  display.println(domaciUcet.length() > 0 ? domaciUcet : iban);
  if (vs.length() > 0) {
    txt_y += 32;
    display.setCursor(18, txt_y);
    display.print("VS: ");
    display.println(vs);
  }
  display.setFont(NULL);
  display.display(true);
}

void showWifiInfo(const char* status, const char* ssid, IPAddress ip) {
  display.fillScreen(GxEPD_WHITE);
  display.setTextSize(2);
  display.setCursor(8, 24);
  display.print("WiFi: ");
  display.println(status);
  display.setTextSize(1);
  int y = 56;
  if (ssid && strlen(ssid) > 0) {
    display.setCursor(8, y);
    display.print("SSID: ");
    display.println(ssid);
    y += 24;
  }
  display.setCursor(8, y);
  display.print("IP: ");
  display.println(ip);
  display.display(true);
}

void setup() {
  Serial.begin(115200);
  Serial.println("--- Spouštím ESP32 QR server ---");

  pinMode(DISPLAY_POWER_PIN, OUTPUT);
  digitalWrite(DISPLAY_POWER_PIN, HIGH);
  delay(500);

  display.init();
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);

  EEPROM.begin(EEPROM_SIZE);

  String wifi_ssid = loadWifiSSIDFromEEPROM();
  String wifi_pass = loadWifiPassFromEEPROM();
  if (wifi_ssid.length() < 2) {
    wifi_ssid = "TVAJE_VYCHOZI_SIT";
    wifi_pass = "TVOJE_HESLO";
  }

  bool wifi_connected = false;
  Serial.print("Připojuji k WiFi: ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

  unsigned long startAttemptTime = millis();
  while (!wifi_connected && millis() - startAttemptTime < 20000) {
    if (WiFi.status() == WL_CONNECTED) {
      wifi_connected = true;
      break;
    }
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (wifi_connected) {
    Serial.print("Připojeno na WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP adresa: ");
    Serial.println(WiFi.localIP());
    showWifiInfo("Pripojeno", WiFi.SSID().c_str(), WiFi.localIP());
  } else {
    WiFi.disconnect(true);
    Serial.println("WiFi se nepodařilo připojit, spouštím Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(softAP_ssid, softAP_pass);
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP SSID: ");
    Serial.println(softAP_ssid);
    Serial.print("AP IP : ");
    Serial.println(apIP);
    showWifiInfo("AP REZIM", softAP_ssid, apIP);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("HTTP GET /");
    String savedIban = loadAccountFromEEPROM();
    request->send(200, "text/html; charset=utf-8", generateHtmlForm(savedIban));
  });

  server.on("/qr", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("HTTP GET /qr");

    String ibanRaw = request->getParam("account")->value();
    String iban = cleanIBAN(ibanRaw);

    String amount = request->getParam("amount")->value();
    String vs = "";
    if (request->hasParam("vs"))
      vs = request->getParam("vs")->value();
    String date = request->getParam("date")->value();

    if (!isValidCzIBAN(iban)) {
      request->send(200, "text/html; charset=utf-8",
        "<!DOCTYPE html><html lang='cs'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1'/>"
        "<title>Chyba - QR platba</title><style>body{font-family:sans-serif;padding:20px;}</style></head><body>"
        "<h3>Chyba: Číslo účtu musí být český IBAN bez mezer a o délce 24 znaků!</h3><p><a href='/'>Zpět</a></p></body></html>");
      Serial.println("Chyba: neplatný IBAN!");
      return;
    }

    if (amount.toFloat() <= 0.0) {
      request->send(200, "text/html; charset=utf-8",
        "<!DOCTYPE html><html lang='cs'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1'/>"
        "<title>Chyba - QR platba</title><style>body{font-family:sans-serif;padding:20px;}</style></head><body>"
        "<h3>Chyba: Částka musí být větší než 0!</h3><p><a href='/'>Zpět</a></p></body></html>");
      Serial.println("Chyba: částka <= 0.");
      return;
    }

    if (vs.length() > 0 && (vs.length() > 10 || vs.toInt() <= 0)) {
      request->send(200, "text/html; charset=utf-8",
        "<!DOCTYPE html><html lang='cs'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1'/>"
        "<title>Chyba - QR platba</title><style>body{font-family:sans-serif;padding:20px;}</style></head><body>"
        "<h3>Chyba: Variabilní symbol musí být číslo do 10 cifer.</h3><p><a href='/'>Zpět</a></p></body></html>");
      Serial.println("Chyba: špatný VS.");
      return;
    }

    String savedIban = loadAccountFromEEPROM();
    if (iban != savedIban) {
      saveAccountToEEPROM(iban);
      Serial.println("Nový IBAN uložen do EEPROM.");
    }

    String platbaQR = generateQRPlatba(iban, amount, date, vs);
    showQRCode(platbaQR, amount, vs, iban);

    String msg = "<!DOCTYPE html><html lang='cs'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1'/>"
      "<title>QR platba</title><style>body{font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; padding:2em; background:#f7fafc; color:#222;}"
      ".message{max-width:320px; margin:3em auto; padding:1.8em 2em; background:#fff; box-shadow:0 4px 18px rgba(0,0,0,0.1); border-radius:12px; text-align:center; font-size:1.15em; line-height:1.4;}"
      "a{display:inline-block; margin-top:1.25em; color:#197dcc; text-decoration:none; font-weight:600;} a:hover{text-decoration:underline;}</style></head><body>"
      "<div class='message'><b>QR kód je zobrazen na displeji!</b><br><a href='/'>Zpět</a></div></body></html>";
    request->send(200, "text/html; charset=utf-8", msg);
    Serial.println("QR kód úspěšně zobrazen na displeji.");
  });

  server.on("/admin", HTTP_GET, [](AsyncWebServerRequest* request) {
    String currSSID = loadWifiSSIDFromEEPROM();
    String currPass = loadWifiPassFromEEPROM();
    String page = generateAdminForm(currSSID, currPass);
    request->send(200, "text/html; charset=utf-8", page);
  });

  server.on("/wifi_save", HTTP_POST, [](AsyncWebServerRequest* request) {
    String ssid = request->getParam("ssid", true)->value();
    String pass = request->getParam("pass", true)->value();
    saveWifiToEEPROM(ssid, pass);
    Serial.println("[WiFi] Uživatelské nastavení ulozeno do EEPROM");
    request->send(200, "text/html; charset=utf-8",
      "<!DOCTYPE html><html lang='cs'><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width, initial-scale=1' />"
      "<title>WiFi Uloženo</title><style>body{font-family:sans-serif;padding:20px;background:#f7fafc;color:#222;text-align:center;}"
      "a{color:#197dcc;text-decoration:none;font-weight:600;} a:hover{text-decoration:underline;}.msgbox{max-width:320px;margin:3em auto;padding:1.8em 1em;background:#fff;box-shadow:0 4px 18px rgba(0,0,0,0.1);border-radius:12px;}"
      "</style></head><body><div class='msgbox'><b>Uloženo! Nové údaje se použijí po restartu zařízení.</b><br><br><a href='/admin'>Zpět na nastavení WiFi</a></div></body></html>");
    delay(1200);
    ESP.restart();
  });

  server.begin();
  Serial.println("Webserver spuštěn.");
}

void loop() {
  // program běží asynchronně přes server
}
