#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

class CacheManager
{
public:
    using UpdateCallback = std::function<void(StaticJsonDocument<2048> &)>;

    CacheManager(unsigned long intervalMs, UpdateCallback updateFunc);

    void begin(); // Optioneel, voor init
    void tick();  // Call this in your loop()

    const String &getCachedString();
    void forceUpdate();

private:
    void updateCache();

    StaticJsonDocument<2048> jsonDoc;
    String cachedString;
    unsigned long lastUpdate = 0;
    unsigned long updateInterval;
    UpdateCallback updateCallback;
};
