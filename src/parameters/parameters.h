#pragma once
#include <Arduino.h>
#include <vector>
#include <map>
#include <set>

// Algemene flags
extern bool wifiPermanentFailure;
extern String currentLanguage;
extern bool parametersFromFile; // Geeft aan of parameters uit bestand geladen zijn

// Timers Namespace: extern gedeclareerd, definitie in .cpp
namespace Timers
{
    extern unsigned long thingSpeak_UpdateInterval;
    extern unsigned long Timers_updateInterval;
    extern unsigned long Timers_forecastInterval;
    extern unsigned long Timers_stableDuration;
    extern unsigned long Timers_wifiReconnectInterval;
    extern unsigned long Timers_flushInterval;
    extern unsigned long Timers_energyLogInterval;
}

// Parameters Namespace: gevoelige en niet-gevoelige parameters extern gedeclareerd
namespace Parameters
{
    extern String ap_ssid;
    extern String ap_password;
    extern String wifi1_ssid;
    extern String wifi1_password;
    extern String wifi2_ssid;
    extern String wifi2_password;
    extern String thingSpeak_Host;
    extern String thingSpeak_ApiKey;

    extern int smoothingWarmupThreshold;
}

// Struct voor parameterreflectie
struct ParameterDescriptor
{
    const char *key;
    void *ptr;
    enum Type
    {
        STRING,
        INT,
        LONG
    } type;
};

// API-functies
std::vector<ParameterDescriptor> getAllParameters();
std::map<String, String> getCurrentParametersAsMap();
void initParameters();
void updateParametersFromMap(const std::map<String, String> &settingsMap);
