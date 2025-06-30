#pragma once
#include <Arduino.h>
#include <SD.h>
#include <ESPAsyncWebServer.h>

class RouteSdCard
{
public:
    static void registerRoutes(AsyncWebServer &server);

private:
    static void handleRoot(AsyncWebServerRequest *request);
    static void handleUploadBody(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
    static String generateHtml(const String &path);
    static String generateHtmlContent(const String &path);
};
