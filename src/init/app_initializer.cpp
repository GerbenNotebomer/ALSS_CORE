#include <Arduino.h>
#include <Wire.h>

#include "app_initializer.h"
#include "../common/common_includes.h"
#include "../logger/logger.h"
#include "../utils/debug/debug_util.h"
#include "../utils/eeprom/eeprom_manager.h"
#include "../hardware/aht20_bmp280/aht20_bmp280_module.h"
#include "../hardware/ina3221/ina3221_module.h"
#include "../hardware/rtc/rtc_pcf8563_module.h"
#include "../parameters/parameters.h"
#include "../language/language_manager.h"
#include "../thingspeak/thing_speak.h"
#include "../wifi_sta_ap/wifi_sta_ap.h"
#include "../webserver/web_server_manager.h"
#include "../webserver/cache_manager.h"

//  --  begin  --  //
void AppInitializer::begin()
{
    Serial.begin(115200);
    delay(1000);
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!SDCardUtils::begin(10))
    {
        DEBUG_LOG("⚠️ Logging valt terug op LittleFS.");
    }

    Logger::begin();

    if (!langManager.init())
    {
        DEBUG_LOG("❌ Taal initialisatie mislukt");
    }

    EEPROM_Manager::loadDefaultsIfNeeded();
    DebugUtil::initDefaultModules();
    EEPROM_Manager::init();

    checkPsram(); // Dit zit in utils/info/info.cpp

    AHT20BMP280_Module::getInstance().begin();
    rtcManager.begin();
    thingSpeak.begin();
    INA3221_Module::getInstance().begin();

    initParameters();
    WiFiManager::begin();
    WebServerManager::begin();
}
