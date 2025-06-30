#include "parameters.h"
#include "../common/common_includes.h"

// Flags en niet-gevoelige globale variabelen
bool wifiPermanentFailure = false;
String currentLanguage = "nl";
bool parametersFromFile = false;

// -- Timers --
// Default waarden kunnen hier blijven staan, niet gevoelig
namespace Timers
{
    unsigned long thingSpeak_UpdateInterval = 60000;
    unsigned long Timers_updateInterval = 1000;
    unsigned long Timers_forecastInterval = 3600000;
    unsigned long Timers_stableDuration = 10000;
    unsigned long Timers_wifiReconnectInterval = 5 * 60 * 1000;
    unsigned long Timers_flushInterval = 5 * 60 * 1000;
    unsigned long Timers_energyLogInterval = 3600000;
}

// -- Parameters --
// Gevoelige default waarden komen extern uit parameters_private.cpp
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

    int smoothingWarmupThreshold = 10; // niet gevoelig, kan hier
}

// -- getAllParameters -- //
std::vector<ParameterDescriptor> getAllParameters()
{
    return {
        {"ap_ssid", &Parameters::ap_ssid, ParameterDescriptor::STRING},
        {"ap_password", &Parameters::ap_password, ParameterDescriptor::STRING},
        {"wifi1_ssid", &Parameters::wifi1_ssid, ParameterDescriptor::STRING},
        {"wifi1_password", &Parameters::wifi1_password, ParameterDescriptor::STRING},
        {"wifi2_ssid", &Parameters::wifi2_ssid, ParameterDescriptor::STRING},
        {"wifi2_password", &Parameters::wifi2_password, ParameterDescriptor::STRING},
        {"thingSpeak_Host", &Parameters::thingSpeak_Host, ParameterDescriptor::STRING},
        {"thingSpeak_ApiKey", &Parameters::thingSpeak_ApiKey, ParameterDescriptor::STRING},
        {"smoothingWarmupThreshold", &Parameters::smoothingWarmupThreshold, ParameterDescriptor::INT},
        {"thingSpeak_UpdateInterval", &Timers::thingSpeak_UpdateInterval, ParameterDescriptor::LONG},
        {"Timers_updateInterval", &Timers::Timers_updateInterval, ParameterDescriptor::LONG},
        {"Timers_forecastInterval", &Timers::Timers_forecastInterval, ParameterDescriptor::LONG},
        {"Timers_stableDuration", &Timers::Timers_stableDuration, ParameterDescriptor::LONG},
        {"Timers_wifiReconnectInterval", &Timers::Timers_wifiReconnectInterval, ParameterDescriptor::LONG},
        {"Timers_flushInterval", &Timers::Timers_flushInterval, ParameterDescriptor::LONG},
        {"Timers_energyLogInterval", &Timers::Timers_energyLogInterval, ParameterDescriptor::LONG}};
}

// -- getCurrentParametersAsMap -- //
std::map<String, String> getCurrentParametersAsMap()
{
    std::map<String, String> map;
    for (const auto &p : getAllParameters())
    {
        switch (p.type)
        {
        case ParameterDescriptor::STRING:
            map[p.key] = *(static_cast<String *>(p.ptr));
            break;
        case ParameterDescriptor::INT:
            map[p.key] = String(*(static_cast<int *>(p.ptr)));
            break;
        case ParameterDescriptor::LONG:
            map[p.key] = String(*(static_cast<unsigned long *>(p.ptr)));
            break;
        }
    }
    return map;
}

// -- setDefaultParameters -- //
static void setDefaultParameters()
{
    parametersFromFile = false;

    // Gevoelige waarden worden niet hier geinitialiseerd, want extern in parameters_private.cpp

    // Niet-gevoelige default waarden
    Parameters::smoothingWarmupThreshold = 10;

    Timers::thingSpeak_UpdateInterval = 60000;
    Timers::Timers_updateInterval = 1000;
    Timers::Timers_forecastInterval = 3600000;
    Timers::Timers_stableDuration = 10000;
    Timers::Timers_wifiReconnectInterval = 5 * 60 * 1000;
    Timers::Timers_flushInterval = 5 * 60 * 1000;
    Timers::Timers_energyLogInterval = 3600000;

    DEBUG_LOGF("[ParametersManager] 🛠️ Standaardparameters ingesteld (exclusief gevoelige data)");
}

// -- writeDefaultParametersToFile -- //
static bool writeDefaultParametersToFile()
{
    if (!SDCardUtils::fileExists("/config") && !SD.mkdir("/config"))
    {
        DEBUG_LOGF("[ParametersManager] ❌ Map /config aanmaken mislukt");
        return false;
    }

    auto paramMap = getCurrentParametersAsMap();
    String content;
    for (const auto &entry : paramMap)
    {
        content += entry.first + "=" + entry.second + "\n";
    }

    return SDCardUtils::writeFile("/config/parameters.txt", content);
}

// -- parseLine -- //
void parseLine(const String &line)
{
    int sep = line.indexOf('=');
    if (sep == -1)
        return;

    String key = line.substring(0, sep);
    key.trim();
    String value = line.substring(sep + 1);
    value.trim();

    for (auto &p : getAllParameters())
    {
        if (key != p.key)
            continue;

        switch (p.type)
        {
        case ParameterDescriptor::STRING:
            *(static_cast<String *>(p.ptr)) = value;
            break;
        case ParameterDescriptor::INT:
            *(static_cast<int *>(p.ptr)) = value.toInt();
            break;
        case ParameterDescriptor::LONG:
            *(static_cast<unsigned long *>(p.ptr)) = value.toInt();
            break;
        }
        break;
    }
}

// -- loadParametersFromFile -- //
void loadParametersFromFile()
{
    setDefaultParameters();

    String content = SDCardUtils::readFile("/config/parameters.txt");
    if (content.isEmpty())
    {
        DEBUG_LOGF("[ParametersManager] ⚠️ parameters.txt is leeg of niet leesbaar");
        return;
    }

    std::set<String> loadedKeys;
    int start = 0, end = 0;

    while ((end = content.indexOf('\n', start)) != -1)
    {
        String line = content.substring(start, end);
        parseLine(line);

        int sep = line.indexOf('=');
        if (sep != -1)
        {
            String insertKey = line.substring(0, sep);
            insertKey.trim();
            loadedKeys.insert(insertKey);
        }

        start = end + 1;
    }

    if (start < content.length())
    {
        String line = content.substring(start);
        parseLine(line);

        int sep = line.indexOf('=');
        if (sep != -1)
        {
            String insertKey = line.substring(0, sep);
            insertKey.trim();
            loadedKeys.insert(insertKey);
        }
    }

    parametersFromFile = true;
    DEBUG_LOGF("[ParametersManager] 📄 Parameters geladen uit bestand");

    auto allParams = getAllParameters();
    bool fileUpdated = false;
    String additionalContent;

    for (const auto &param : allParams)
    {
        if (loadedKeys.find(param.key) == loadedKeys.end())
        {
            fileUpdated = true;

            switch (param.type)
            {
            case ParameterDescriptor::STRING:
                additionalContent += String(param.key) + "=" + *(static_cast<String *>(param.ptr)) + "\n";
                break;
            case ParameterDescriptor::INT:
                additionalContent += String(param.key) + "=" + String(*(static_cast<int *>(param.ptr))) + "\n";
                break;
            case ParameterDescriptor::LONG:
                additionalContent += String(param.key) + "=" + String(*(static_cast<unsigned long *>(param.ptr))) + "\n";
                break;
            }

            DEBUG_LOGF("[ParametersManager] ➕ Missende parameter toegevoegd: %s (defaultwaarde)", param.key);
        }
    }

    if (fileUpdated)
    {
        SDCardUtils::appendToFile("/config/parameters.txt", additionalContent);
        DEBUG_LOGF("[ParametersManager] 💾 parameters.txt bijgewerkt met ontbrekende waarden");
    }
}

// -- initParameters -- //
void initParameters()
{
    if (!SDCardUtils::fileExists("/config/parameters.txt"))
    {
        DEBUG_LOGF("[ParametersManager] 📁 parameters.txt niet gevonden, aanmaken...");
        setDefaultParameters();
        if (!writeDefaultParametersToFile())
        {
            DEBUG_LOGF("[ParametersManager] ❌ Schrijven naar parameters.txt mislukt");
        }
    }
    else
    {
        loadParametersFromFile();
    }

    SDCardUtils::ensureLogDirectoriesExist();
}

// -- updateParametersFromMap -- //
void updateParametersFromMap(const std::map<String, String> &settingsMap)
{
    for (const auto &kv : settingsMap)
    {
        String key = kv.first;
        String value = kv.second;

        for (auto &p : getAllParameters())
        {
            if (key != p.key)
                continue;

            switch (p.type)
            {
            case ParameterDescriptor::STRING:
                *(static_cast<String *>(p.ptr)) = value;
                break;
            case ParameterDescriptor::INT:
                *(static_cast<int *>(p.ptr)) = value.toInt();
                break;
            case ParameterDescriptor::LONG:
                *(static_cast<unsigned long *>(p.ptr)) = value.toInt();
                break;
            }
            break;
        }
    }
}
