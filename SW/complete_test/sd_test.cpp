#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "sd_test.h"
#include "board_pins.h"

#define SDTEST_TEXT_PADDING 25

// Nepoužívej volání begin() v globálním prostoru.
// Vystačíme si se sdíleným SPI, jen mu nastavíme piny uvnitř SDInit().

static int appendFile(fs::FS &fs, const char *path, const char *message) {
  File file = fs.open(path, FILE_APPEND);
  if (!file) return -1;
  bool ok = file.print(message);
  file.close();
  return ok ? 0 : -2;
}

static int writeFile(fs::FS &fs, const char *path, const char *message) {
  File file = fs.open(path, FILE_WRITE);
  if (!file) return -1;
  bool ok = file.print(message);
  file.close();
  return ok ? 0 : -2;
}

static int readFile(fs::FS &fs, const char *path) {
  uint16_t lines = 0;
  File file = fs.open(path);
  if (!file) return -1;
  while (file.available()) {
    if (file.read() == '\n') lines++;
  }
  file.close();
  return lines;
}

static void print_middle_text(
  GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display,
  const char *text, uint16_t y
) {
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  existing_display.setFont(&TomThumb);
  existing_display.setTextSize(3);
  existing_display.setTextColor(GxEPD_BLACK);
  existing_display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  existing_display.setCursor((existing_display.width() - tbw) / 2, y);
  existing_display.print(text);
}

static void print_last_line(
  GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display,
  int16_t y, fs::FS &fs, const char *path
) {
  int lines = readFile(fs, path);
  int actLines = 1;
  char disp[64] = {0};
  File file = fs.open(path);
  if (!file) {
    snprintf(disp, sizeof(disp), "Unable to open file");
    print_middle_text(existing_display, disp, y);
    return;
  }

  // Posuň se na začátek posledního řádku
  while (file.available() && actLines < lines) {
    if (file.read() == '\n') actLines++;
  }

  // Přečti poslední řádek bezpečně
  char last[48] = {0};
  size_t i = 0;
  while (file.available() && i < sizeof(last) - 1) {
    char t = file.read();
    if (t == '\r' || t == '\n') break;
    last[i++] = t;
  }
  file.close();

  snprintf(disp, sizeof(disp), "Last line written: %s", last);
  print_middle_text(existing_display, disp, y);
}

static int SDInit(
  GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display, int16_t y
) {
  char disp[48];

  // Nastav piny pro sdílené SPI a teprve pak mountni SD
  SPI.begin(SCK, MISO, MOSI, SD_CS);
  // můžeš přidat i frekvenci, např. 20 MHz:
  if (!SD.begin(SD_CS, SPI /*, 20000000 */)) {
    return -1;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) return -1;

  uint64_t cardSizeMB = SD.cardSize() / (1024ULL * 1024ULL);

  const char *typeStr = "UNKNOWN";
  if      (cardType == CARD_MMC)  typeStr = "MMC";
  else if (cardType == CARD_SD)   typeStr = "SDSC";
  else if (cardType == CARD_SDHC) typeStr = "SDHC";

  snprintf(disp, sizeof(disp), "SD Card Type: %s, size:", typeStr);
  print_middle_text(existing_display, disp, y);

  // pozor na formát 64-bit čísla
  char sizeBuf[32];
  snprintf(sizeBuf, sizeof(sizeBuf), "%llu Mb", (unsigned long long)cardSizeMB);
  print_middle_text(existing_display, sizeBuf, y * 2);

  return 0;
}

void SDTest(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display) {
  char disp[64];
  existing_display.fillScreen(GxEPD_WHITE);

  if (SDInit(existing_display, SDTEST_TEXT_PADDING)) {
    snprintf(disp, sizeof(disp), "SD card not found");
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING);
    SD.end();
    existing_display.display(true);
    return;
  }

  snprintf(disp, sizeof(disp), "SD card mounted");
  print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 3);

  File file = SD.open("/test.txt", FILE_APPEND);
  if (!file) {
    if (writeFile(SD, "/test.txt", "test\n")) {
      snprintf(disp, sizeof(disp), "Unable to write into file");
      print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);
      existing_display.display(true);
      return;
    }
    snprintf(disp, sizeof(disp), "File test.txt created, test line written");
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);

    snprintf(disp, sizeof(disp), "Number of lines in the document: %d", readFile(SD, "/test.txt"));
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 5);
    print_last_line(existing_display, SDTEST_TEXT_PADDING * 6, SD, "/test.txt");
  } else {
    file.close(); // FILE_APPEND se otevřel OK, ale pro čtení počtu řádků otevřeme zvlášť

    snprintf(disp, sizeof(disp), "Number of lines in the");
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);
    snprintf(disp, sizeof(disp), "document: %d", readFile(SD, "/test.txt"));
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 5);

    if (appendFile(SD, "/test.txt", "test\n")) {
      snprintf(disp, sizeof(disp), "Unable to append into test.txt");
      print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 6);
      existing_display.display(true);
      return;
    }

    print_middle_text(existing_display, "Test line appended to", SDTEST_TEXT_PADDING * 6);
    print_middle_text(existing_display, "test.txt", SDTEST_TEXT_PADDING * 7);

    snprintf(disp, sizeof(disp), "Number of lines in the");
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 8);
    snprintf(disp, sizeof(disp), "document: %d", readFile(SD, "/test.txt"));
    print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 9);

    print_last_line(existing_display, SDTEST_TEXT_PADDING * 10, SD, "/test.txt");
  }

  SD.end();
  existing_display.display(true);
}