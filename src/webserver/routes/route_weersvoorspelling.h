#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class RouteWeersvoorspelling
{
public:
    static void registerRoutes(AsyncWebServer &server);

private:
    static void handlePage(AsyncWebServerRequest *request);
    static void handleApi(AsyncWebServerRequest *request);
    static String generatePage(); // correcte versie zonder parameters
};
