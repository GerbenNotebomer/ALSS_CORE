#pragma once

#include <Arduino.h>
#include <RTClib.h>

class RTCManager
{
public:
    RTCManager();
    void begin();
    void syncTimeWithNTP();
    String getFormattedDateTime() const;
    String getDag() const;
    DateTime now();

private:
    RTC_PCF8563 rtc;
};

extern RTCManager rtcManager;
