#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include "../debug/debug_util.h"
//  --  EEPROM_Manager  --  //
class EEPROM_Manager
{
public:
    static constexpr int EEPROM_SIZE = 512;
    static constexpr int EEPROM_VERSION = 1;

    static constexpr int ADDR_VERSION = 0;
    static constexpr int ADDR_CALIBRATION = ADDR_VERSION + sizeof(int);
    static constexpr int ADDR_TOTAL_AH = 300;
    static constexpr int ADDR_CAPACITY = 400;
    static constexpr int ADDR_DEBUG_SETTINGS = 450;
    static constexpr int ADDR_LANGUAGE = 500;
    static constexpr int LANG_CODE_LENGTH = 8; // bijv. "en", "nl", "fr"
    // Public interface (alle methoden zijn static)
    static void updateDebugSettingsFromDebugUtil();
    static void saveDebugSettingsToEEPROM();
    static void loadDebugSettingsFromEEPROM();

    static void saveCalibrationToEEPROM();
    static void loadCalibrationFromEEPROM();

    static void saveAhToEEPROM();
    static void loadAhFromEEPROM();

    static void saveCapacityToEEPROM();
    static void loadCapacityFromEEPROM();

    static void factoryResetEEPROM();
    static bool isEEPROMVersionValid();
    static void loadDefaultsIfNeeded();

    static void saveLanguageToEEPROM(const String &langCode);
    static String loadLanguageFromEEPROM();
    static void init();

private:
    struct DebugSettings
    {
        bool moduleEnabled[MAX_DEBUG_MODULES];
    };
    static DebugSettings debugSettings;
};
