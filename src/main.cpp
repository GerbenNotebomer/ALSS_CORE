
#include <Arduino.h>
#include "init/app_initializer.h"
#include "loop/loop_tasks.h"
#include "webserver/routes/route_json.h"
#include "webserver/cache_manager.h"

// Globale pointer naar de JSON cache
CacheManager *dataJsonCache = nullptr;

void setup()
{
    // Initialiseer alles (WiFi, hardware, logging, etc.)
    AppInitializer::begin();

    // Instantieer de cache manager met update-interval en JSON generator-functie
    dataJsonCache = new CacheManager(1000, RouteJson::buildDataJSON);
    dataJsonCache->begin();

    // Geef de cache manager door aan de loop-taken
    LoopTasks::setCache(dataJsonCache);
}

void loop()
{
    LoopTasks::handlePeriodicUpdates(); // Sensoren + logging + flush
    LoopTasks::handleThingSpeak();      // ThingSpeak updates (indien verbonden)
    LoopTasks::handleWifi();            // Reconnect check
    LoopTasks::printDebugEveryMinute(); // Debug info 1x per minuut
}
