#pragma once

#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include <SPI.h>
#include <LittleFS.h>
#include <vector>
#include "hardware/rtc/rtc_pcf8563_module.h"

class SDCardUtils
{
public:
    // Initialisatie van SD-kaart, of fallback op LittleFS
    static bool begin(uint8_t csPin = 10);
    static bool isUsingSD();
    // Bestand bewerkingen
    static bool fileExists(const String &path);
    static bool writeFile(const String &path, const String &content, bool append = false);
    static String readFile(const String &path);
    static bool deleteFile(const String &path);
    static String getDirectoryListing(const String &dir, uint8_t levels = 0);

    // Loggen van energiewaarden
    static void logEnergyToSD(const DateTime &now);
    static void flushLogBuffer(); // <-- Buffer handmatig wegschrijven

    // Hulpjes
    static String formatTwoDigits(int val);
    static String getLogFilePath(const DateTime &dt);
    static void ensureLogDirectoriesExist();
    static bool appendToFile(const char *path, const String &data);
};

/*
#pragma once
#include <Arduino.h>
#include <SD.h>
#include <time.h>
#include "hardware/RTC_PCF8563_Module.h"
#include <FS.h>
#include <LittleFS.h>
#include <SPI.h>
#include <vector>

class SDCardUtils
{
public:
    static bool begin(uint8_t csPin = 10);
    static bool fileExists(const String &path);
    static bool writeFile(const String &path, const String &content, bool append = false);
    static String readFile(const String &path);
    static bool deleteFile(const String &path);
    static void listFiles(const String &dir = "/", uint8_t levels = 0);
    static String getDirectoryListing(const String &dir, uint8_t levels);
    static String formatTwoDigits(int val);
    static String getLogFilePath(const DateTime &dt, const String &channelName);
    static void ensureLogDirectoriesExist();
    static void logEnergyToSD(const DateTime &now);
};
*/
