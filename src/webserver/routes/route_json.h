#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "webserver/cache_manager.h"
extern CacheManager *dataJsonCache;

class RouteJson
{
public:
    static void buildDataJSON(StaticJsonDocument<2048> &jsonDoc);
    static void registerRoutes(AsyncWebServer &server);
};