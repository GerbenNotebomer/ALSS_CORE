#include "loop_tasks.h"
#include "../logger/logger.h"
#include "../thingspeak/thing_speak.h"
#include "../wifi_sta_ap/wifi_sta_ap.h"
#include "../hardware/ina3221/ina3221_module.h"
#include "../hardware/rtc/rtc_pcf8563_module.h"
#include "../hardware/sd_card/sd_card_module.h"
#include "../parameters/parameters.h"
#include "../webserver/cache_manager.h"

static unsigned long previousMillis = 0;
static unsigned long lastFlushCheck = 0;
static unsigned long lastPrint = 0;

// Definitie van de static member
std::function<void()> LoopTasks::cacheTickFn = nullptr;

void LoopTasks::setCache(void *cache)
{
    if (cache)
    {
        CacheManager *cm = static_cast<CacheManager *>(cache);
        cacheTickFn = [cm]()
        {
            cm->tick();
        };
    }
    else
    {
        cacheTickFn = nullptr;
    }
}

void LoopTasks::handlePeriodicUpdates()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= Timers::Timers_updateInterval)
    {
        previousMillis = currentMillis;

        DateTime now = rtcManager.now();
        INA3221_Module::getInstance().updateAllChannels();
        Logger::tick(currentMillis, now);

        if (currentMillis - lastFlushCheck > Timers::Timers_flushInterval)
        {
            lastFlushCheck = currentMillis;
            SDCardUtils::flushLogBuffer();
        }
    }

    if (cacheTickFn)
        cacheTickFn();
}

void LoopTasks::handleThingSpeak()
{
    if (!wifiPermanentFailure)
    {
        thingSpeak.update(millis());
    }
}

void LoopTasks::handleWifi()
{
    if (!wifiPermanentFailure)
    {
        WiFiManager::checkReconnect();
    }
}

void LoopTasks::printDebugEveryMinute()
{
    if (millis() - lastPrint > 60000)
    {
        lastPrint = millis();
        WiFiManager::printConnectedClients();
    }
}
