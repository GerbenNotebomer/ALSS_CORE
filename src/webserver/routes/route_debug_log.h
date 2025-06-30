#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class RouteDebugLog
{
public:
    static void handleDebugLogPage(AsyncWebServerRequest *request);
    static void handleDebugLogJson(AsyncWebServerRequest *request);
    static void handleDebugSettingsPost(AsyncWebServerRequest *request);
    static void registerRoutes(AsyncWebServer &server);
    static void handleDebugClearPost(AsyncWebServerRequest *request);
};