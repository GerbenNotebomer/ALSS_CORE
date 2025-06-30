#include "cache_manager.h"

CacheManager::CacheManager(unsigned long intervalMs, UpdateCallback updateFunc)
    : updateInterval(intervalMs), updateCallback(updateFunc)
{
}

void CacheManager::begin()
{
    updateCache();
}

void CacheManager::tick()
{
    if (millis() - lastUpdate >= updateInterval)
    {
        updateCache();
    }
}

void CacheManager::forceUpdate()
{
    updateCache();
}

void CacheManager::updateCache()
{
    jsonDoc.clear();
    if (updateCallback)
    {
        updateCallback(jsonDoc);
    }
    cachedString = "";
    serializeJson(jsonDoc, cachedString);
    lastUpdate = millis();
}

const String &CacheManager::getCachedString()
{
    return cachedString;
}
