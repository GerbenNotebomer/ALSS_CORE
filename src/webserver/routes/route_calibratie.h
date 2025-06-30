#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

namespace RouteCalibration
{
    void registerRoutes(AsyncWebServer &server);
    void handleCalibratie(AsyncWebServerRequest *request);
}

bool getArgFloat(AsyncWebServerRequest *request, int ch, const char *suffix, float &outVal);
bool getArgBool(AsyncWebServerRequest *request, int ch, const char *suffix, bool &outVal);