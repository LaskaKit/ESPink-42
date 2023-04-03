/* SD test for LaskaKit ESPink-4.2"
 *
 * -------- ESPink pinout -------
 * MOSI/SDI 23
 * CLK/SCK 18
 * SS 5 //CS
 * DC 17  
 * RST 16  
 * BUSY 4 
 * -------------------------------
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SDTEST_TEXT_PADDING 25

#define TIME_TO_SLEEP 5			  // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups

#define DISPLAY_POWER_PIN 2 // Epaper power pin

#define SD_CS 27
SPIClass spiSD(HSPI); // Use HSPI for SD card

GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEW042T2 400x300, UC8176 (IL0398)

void start_sleep()
{
	gpio_hold_en((gpio_num_t)DISPLAY_POWER_PIN); // Hold Epaper on during sleep
	gpio_deep_sleep_hold_en();					 // Hold Epaper on during sleep
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
	display.setFont(&FreeMonoBold12pt7b);
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
	if (!SD.begin(SD_CS, spiSD))
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
	pinMode(DISPLAY_POWER_PIN, OUTPUT);	   // Set epaper transistor as output
	digitalWrite(DISPLAY_POWER_PIN, HIGH); // Turn on epaper transistor
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
