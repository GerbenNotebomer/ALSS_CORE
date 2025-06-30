#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

class WebServerManager
{
public:
    static void begin();
    static AsyncWebServer server;
};
