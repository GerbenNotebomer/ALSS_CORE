#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>

class RouteSettings
{
public:
    static void registerRoutes(AsyncWebServer &server);
    static String beautifyLabel(const String &raw, const String &prefix);
};