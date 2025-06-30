#include "logger.h"
#include "../utils/debug/debug_util.h"
#include "../hardware/sd_card/sd_card_module.h"
#include "../parameters/parameters.h"

namespace
{
    unsigned long lastFlushCheck = 0;
    unsigned long lastEnergyLogMillis = 0;
}

void Logger::begin()
{
    if (!SDCardUtils::begin(10))
    {
        DEBUG_LOG("⚠️ Logging valt terug op LittleFS.");
    }
}

void Logger::logEnergyIfNeeded(unsigned long millisNow, const DateTime &now)
{
    if (millisNow - lastEnergyLogMillis >= Timers::Timers_energyLogInterval)
    {
        lastEnergyLogMillis = millisNow;
        SDCardUtils::logEnergyToSD(now);
        SDCardUtils::flushLogBuffer();
        DEBUG_LOGF("[EnergyLog] ⚡ Energie gelogd op %02d:%02d:%02d", now.hour(), now.minute(), now.second());
    }
}

void Logger::tick(unsigned long millisNow, const DateTime &now)
{
    Logger::logEnergyIfNeeded(millisNow, now);

    if (millisNow - lastFlushCheck > Timers::Timers_flushInterval)
    {
        lastFlushCheck = millisNow;
        SDCardUtils::flushLogBuffer();
    }
}
