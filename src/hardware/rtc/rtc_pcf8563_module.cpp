#include "rtc_pcf8563_module.h"
#include "../../common/common_includes.h"
#include "../../wifi_sta_ap/wifi_sta_ap.h"

#include <time.h>

RTCManager rtcManager;

RTCManager::RTCManager() {}

////  ---- RTCManager  ----  ////
//  --  begin  --  //
void RTCManager::begin()
{
    if (!rtc.begin())
    {
        DEBUG_LOG("❌ RTC niet gevonden!");
    }
    else
    {
        DEBUG_LOG("✅ RTC gestart");
    }
}

//  --  syncTimeWithNTP  --  //
void RTCManager::syncTimeWithNTP()
{
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000))
    {
        DEBUG_LOGF("[syncTimeWithNTP] Lokale tijd (NL): %02d:%02d:%02d %02d-%02d-%04d",
                   timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                   timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);

        DateTime ntpTime(
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec);

        rtc.adjust(ntpTime);
        DEBUG_LOG("[syncTimeWithNTP] RTC succesvol gesynchroniseerd.");
    }
    else
    {
        DEBUG_LOG("[syncTimeWithNTP] Tijd synchronisatie mislukt.");
    }
}

//  --  getFormattedDateTime  --  //
String RTCManager::getFormattedDateTime() const
{
    DateTime now = rtcManager.now();

    String day = (now.day() < 10) ? "0" + String(now.day()) : String(now.day());
    String month = (now.month() < 10) ? "0" + String(now.month()) : String(now.month());
    String hour = (now.hour() < 10) ? "0" + String(now.hour()) : String(now.hour());
    String minute = (now.minute() < 10) ? "0" + String(now.minute()) : String(now.minute());
    String second = (now.second() < 10) ? "0" + String(now.second()) : String(now.second());

    return day + "/" + month + "/" + String(now.year()) + " " + hour + ":" + minute + ":" + second;
}

//  --  getDag  --  //
String RTCManager::getDag() const
{
    DateTime now = rtcManager.now();

    static const char *dagKeys[] = {
        "dagen.zondag",
        "dagen.maandag",
        "dagen.dinsdag",
        "dagen.woensdag",
        "dagen.donderdag",
        "dagen.vrijdag",
        "dagen.zaterdag"};

    int dagIndex = now.dayOfTheWeek(); // 0 = Zondag, 1 = Maandag, etc.

    if (dagIndex < 0 || dagIndex > 6)
        return ""; // fallback lege string bij fout

    return langManager.get(dagKeys[dagIndex]);
}

//  --  now  --  //
DateTime RTCManager::now()
{
    return rtc.now();
}
