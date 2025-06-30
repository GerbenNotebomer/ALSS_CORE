#pragma once

#include <Arduino.h>

#include <functional>

class LoopTasks
{
public:
    static void setCache(void *cache);
    static void handlePeriodicUpdates();
    static void handleThingSpeak();
    static void handleWifi();
    static void printDebugEveryMinute();

private:
    static std::function<void()> cacheTickFn;
};
