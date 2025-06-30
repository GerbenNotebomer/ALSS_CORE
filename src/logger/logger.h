#pragma once
#include <Arduino.h>
#include <RTClib.h> // Of wat je gebruikt voor DateTime

namespace Logger
{
    void begin();
    void tick(unsigned long millisNow, const DateTime &now);
    void logEnergyIfNeeded(unsigned long millisNow, const DateTime &now);
}
