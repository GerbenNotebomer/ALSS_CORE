#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

class RouteReboot
{
public:
    static void registerRoutes(AsyncWebServer &server);
    static void scheduleReboot(float delaySeconds = 2.0);

private:
    static Ticker rebootTimer;
};