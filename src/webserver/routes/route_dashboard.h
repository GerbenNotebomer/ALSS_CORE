#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace RouteDashboard
{
    void registerRoutes(AsyncWebServer &server);
    void handleDashboard(AsyncWebServerRequest *request);
}