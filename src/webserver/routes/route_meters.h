#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <vector>

class RouteMeters
{
public:
    static void registerRoutes(AsyncWebServer &server);

private:
    static void handleMeters(AsyncWebServerRequest *request);
};
