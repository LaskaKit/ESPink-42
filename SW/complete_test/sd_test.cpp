#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/TomThumb.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "sd_test.h"

#define SDTEST_TEXT_PADDING 25
#define SD_CS 27

SPIClass spiSD(HSPI); // Use HSPI for SD card

static int appendFile(fs::FS &fs, const char *path, const char *message)
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

static int writeFile(fs::FS &fs, const char *path, const char *message)
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

static int readFile(fs::FS &fs, const char *path)
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

static void print_middle_text(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display, char *text, uint16_t padding)
{
	int16_t tbx, tby;
	uint16_t tbw, tbh;
	existing_display.setFont(&TomThumb);
	existing_display.setTextSize(3);
	existing_display.setTextColor(GxEPD_BLACK);
	existing_display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
	existing_display.setCursor((existing_display.width() - tbw) / 2, padding);
	existing_display.print(text);
}

static void print_last_line(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display, int16_t padding, fs::FS &fs, const char *path)
{
	int lines = readFile(fs, path);
	int actLines = 1;
	char text[10] = {0};
	char disp[40] = {0};
	File file = fs.open(path);
	if (!file)
	{
		sprintf(disp, "Unable to open file");
		print_middle_text(existing_display, disp, padding);
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
	print_middle_text(existing_display, disp, padding);
}

static int SDInit(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display, int16_t padding)
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
		print_middle_text(existing_display, disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(existing_display, disp, padding * 2);
	}
	else if (cardType == CARD_SD)
	{
		sprintf(disp, "SD Card Type: SDSC, size:");
		print_middle_text(existing_display, disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(existing_display, disp, padding * 2);
	}
	else if (cardType == CARD_SDHC)
	{
		sprintf(disp, "SD Card Type: SDHC, size:");
		print_middle_text(existing_display, disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(existing_display, disp, padding * 2);
	}
	else
	{
		sprintf(disp, "SD Card Type: UNKNOWN, size:");
		print_middle_text(existing_display, disp, padding);
		sprintf(disp, "%d Mb", cardSize);
		print_middle_text(existing_display, disp, padding * 2);
	}
	return 0;
}

void SDTest(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display)
{
	char disp[50];
	existing_display.fillScreen(GxEPD_WHITE);
	if (SDInit(existing_display, SDTEST_TEXT_PADDING))
	{
		sprintf(disp, "SD card not found");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING);
		SD.end();
		existing_display.display(true);
		return;
	}
	sprintf(disp, "SD card mounted");
	print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 3);
	File file = SD.open("/test.txt", FILE_APPEND);
	if (!file)
	{
		if (writeFile(SD, "/test.txt", "test\n"))
		{
			sprintf(disp, "Unable to write into file");
			print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);
			existing_display.display(true);
			return;
		}
		sprintf(disp, "File test.txt created, test line written");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);
		sprintf(disp, "Number of lines in the document: %d", readFile(SD, "/test.txt"));
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 5);
		print_last_line(existing_display, SDTEST_TEXT_PADDING * 5, SD, "/test.txt");
	}
	else
	{
		sprintf(disp, "Number of lines in the");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 4);
		sprintf(disp, "document: %d", readFile(SD, "/test.txt"));
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 5);
		if (appendFile(SD, "/test.txt", "test\n"))
		{
			sprintf(disp, "Unable to append into test.txt");
			print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 6);
			existing_display.display(true);
			return;
		}
		sprintf(disp, "Test line appended to");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 6);
		sprintf(disp, "test.txt");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 7);
		sprintf(disp, "Number of lines in the");
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 8);
		sprintf(disp, "document: %d", readFile(SD, "/test.txt"));
		print_middle_text(existing_display, disp, SDTEST_TEXT_PADDING * 9);
		print_last_line(existing_display, SDTEST_TEXT_PADDING * 10, SD, "/test.txt");
	}
	SD.end();
	existing_display.display(true);
}