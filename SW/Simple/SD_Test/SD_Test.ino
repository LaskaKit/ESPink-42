/* SD test for LaskaKit ESPink-4.26"
 * 
 * Board:   LaskaKit ESPink-4.26   https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 *
 * Libraries:
 * EPD library: https://github.com/ZinggJM/GxEPD2
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <driver/rtc_io.h>

#define SDTEST_TEXT_PADDING 25

#define TIME_TO_SLEEP 5			  // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

// nefunguje
SPIClass spiSD(HSPI); // Use HSPI for SD card

//#define ESPink42_V2     //for version v2.4 and earlier
#define ESPink42_V3     //for version v3.0 and above

#ifdef ESPink42_V2
  //MOSI/SDI    23
  //CLK/SCK     18
  //SS/CS       5
  #define DC    17 
  #define RST   16  
  #define BUSY  4 
  #define POWER 2
	#define SD_CS 27
  #define SDA   21
  #define SCL   22
  #define BAT   34
  #define DISPLAY_LED 26      // Display backlight pin
#else ESPink42_V3
  //MOSI/SDI    11
  //CLK/SCK     12
  //SS/CS       10
  #define DC    48 
  #define RST   45  
  #define BUSY  38 
  #define POWER 47
	#define SD_CS 17
  #define SDA   42
  #define SCL   2
  #define BAT   9
  #define DISPLAY_LED 1      // Display backlight pin
#endif

// E-paper display
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(SS, DC, RST, BUSY)); //GDEY042T81 (GDEY042T81-FT02), 400x300, SSD1683 (no inking)
//GxEPD2_4C<GxEPD2_420c_GDEY0420F51, GxEPD2_420c_GDEY0420F51::HEIGHT> display(GxEPD2_420c_GDEY0420F51(SS, DC, RST, BUSY)); // GDEY0420F51 400x300, HX8717 (no inking)
//GxEPD2_3C<GxEPD2_420c_GDEY042Z98, GxEPD2_420c_GDEY042Z98::HEIGHT> display(GxEPD2_420c_GDEY042Z98(SS, DC, RST, BUSY)); // GDEY042Z98 400x300, SSD1683 (no inking)


void start_sleep()
{
  gpio_hold_en((gpio_num_t)POWER); // Hold Epaper on during sleep
  gpio_deep_sleep_hold_en();                   // Hold Epaper on during sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

int appendFile(fs::FS &fs, const char *path, const char *message)
{
	File file = fs.open(path, FILE_APPEND);
	if (!file)
	{
		return -1;
	}
	if (file.print(message))
	{
		file.close();
	}
	else
	{
		file.close();
		return -2;
	}
	return 0;
}

int writeFile(fs::FS &fs, const char *path, const char *message)
{
	File file = fs.open(path, FILE_WRITE);
	if (!file)
	{
		return -1;
	}
	if (file.print(message))
	{
		file.close();
	}
	else
	{
		file.close();
		return -2;
	}
	return 0;
}

int readFile(fs::FS &fs, const char *path)
{
	uint16_t lines = 0;
	File file = fs.open(path);
	if (!file)
	{
		return -1;
	}
	while (file.available())
	{
		if (file.read() == '\n')
		{
			lines++;
		}
	}
	file.close();
	return lines;
}

void print_middle_text(char *text, uint16_t padding)
{
	int16_t tbx, tby;
	uint16_t tbw, tbh;
	display.setFont(&FreeMonoBold9pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
	display.setCursor((display.width() - tbw) / 2, padding);
	display.print(text);
}

void print_last_line(int16_t padding, fs::FS &fs, const char *path)
{
	int lines = readFile(fs, path);
	int actLines = 1;
	char text[10] = {0};
	char disp[40] = {0};
	File file = fs.open(path);
	if (!file)
	{
		sprintf(disp, "Unable to open file");
		print_middle_text(disp, padding);
		return;
	}
	while (actLines < lines)
	{
		char t = file.read();
		if (t == '\n')
		{
			actLines++;
		}
	}
	int i = 0;
	while (file.available())
	{
		char t = file.read();
		text[i++] = t;
	}
	file.close();
	sprintf(disp, "Last line written: %s", text);
	print_middle_text(disp, padding);
}

int SDtestInit(int16_t padding)
{
	char disp[50];
	uint8_t cardType;
	uint64_t cardSize;
	if (!SD.begin(SD_CS))
		{
			return -1;
		}
	cardType = SD.cardType();
	if (cardType == CARD_NONE)
	{
		return -1;
	}
	cardSize = SD.cardSize() / (1024 * 1024);
	if (cardType == CARD_MMC)
	{
		sprintf(disp, "SD Card Type: MMC, size:");
		print_middle_text(disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(disp, padding * 2);
	}
	else if (cardType == CARD_SD)
	{
		sprintf(disp, "SD Card Type: SDSC, size:");
		print_middle_text(disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(disp, padding * 2);
	}
	else if (cardType == CARD_SDHC)
	{
		sprintf(disp, "SD Card Type: SDHC, size:");
		print_middle_text(disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(disp, padding * 2);
	}
	else
	{
		sprintf(disp, "SD Card Type: UNKNOWN, size:");
		print_middle_text(disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(disp, padding * 2);
	}
	return 0;
}

void SDtest()
{
	char disp[50];
	display.fillScreen(GxEPD_WHITE);
	if (SDtestInit(SDTEST_TEXT_PADDING))
	{
		sprintf(disp, "SD card not found");
		print_middle_text(disp, SDTEST_TEXT_PADDING);
		SD.end();
		display.display(true);
		return;
	}
	sprintf(disp, "SD card mounted");
	print_middle_text(disp, SDTEST_TEXT_PADDING * 3);
	File file = SD.open("/test.txt", FILE_APPEND);
	if (!file)
	{
		if (writeFile(SD, "/test.txt", "test\n"))
		{
			sprintf(disp, "Unable to write into file");
			print_middle_text(disp, SDTEST_TEXT_PADDING * 4);
			display.display(true);
			return;
		}
		sprintf(disp, "File test.txt created, test line written");
		print_middle_text(disp, SDTEST_TEXT_PADDING * 4);
		sprintf(disp, "Number of lines in the document: %d", readFile(SD, "/test.txt"));
		print_middle_text(disp, SDTEST_TEXT_PADDING * 5);
		print_last_line(SDTEST_TEXT_PADDING * 5, SD, "/test.txt");
	}
	else
	{
		sprintf(disp, "Number of lines in the");
		print_middle_text(disp, SDTEST_TEXT_PADDING * 4);
		sprintf(disp, "document: %d", readFile(SD, "/test.txt"));
		print_middle_text(disp, SDTEST_TEXT_PADDING * 5);
		if (appendFile(SD, "/test.txt", "test\n"))
		{
			sprintf(disp, "Unable to append into test.txt");
			print_middle_text(disp, SDTEST_TEXT_PADDING * 6);
			display.display(true);
			return;
		}
		sprintf(disp, "Test line appended to");
		print_middle_text(disp, SDTEST_TEXT_PADDING * 6);
		sprintf(disp, "test.txt");
		print_middle_text(disp, SDTEST_TEXT_PADDING * 7);
		sprintf(disp, "Number of lines in the");
		print_middle_text(disp, SDTEST_TEXT_PADDING * 8);
		sprintf(disp, "document: %d", readFile(SD, "/test.txt"));
		print_middle_text(disp, SDTEST_TEXT_PADDING * 9);
		print_last_line(SDTEST_TEXT_PADDING * 10, SD, "/test.txt");
	}
	SD.end();
	display.display(true);
}

void setup()
{
	Serial.begin(115200);
	pinMode(POWER, OUTPUT);	   // Set epaper transistor as output
	digitalWrite(POWER, HIGH); // Turn on epaper transistor
	delay(100);							   // Delay so it has time to turn on
	if (bootCount)						   // If first boot, redraw
	{
		display.init(0, false);
	}
	else
	{
		display.init(0, true);
	}
	display.setRotation(0);
	display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update
	SDtest();
	bootCount++; // Increment boot number every reboot
	start_sleep();
}

void loop()
{
}
