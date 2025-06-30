#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

class ThingSpeak
{
public:
    ThingSpeak();

    void begin();                   // <-- toevoegen
    void update(unsigned long now); // zoals eerder

private:
    void sendToThingSpeak();

    unsigned long lastThingSpeakUpdate;
};

extern ThingSpeak thingSpeak;