#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

// HTML generators
String generateRootPage(const String &boardName, const String &systemInfo);

namespace RouteRoot
{
    void registerRoutes(AsyncWebServer &server);
    void handleRoot(AsyncWebServerRequest *request);
}
