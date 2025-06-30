#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <Update.h>

extern Ticker rebootTimer;

namespace RouteOTA
{
    void registerRoutes(AsyncWebServer &server);
}