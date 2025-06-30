#include "debug_util.h"

std::vector<String> DebugUtil::logBuffer;
const size_t DebugUtil::MAX_LOG_LINES = 100;

bool DebugUtil::debugEnabled = true;
DebugUtil::ModuleDebug DebugUtil::modules[MAX_MODULES] = {};

//  --  moduleNames  --  //
const char *moduleNames[MAX_DEBUG_MODULES] = {
    "main.cpp",
    "language_manager.cpp",
    "sd_card_module.cpp",
    "route_meters.cpp",
    "route_sd_card.cpp",
    "aht20_bmp280_module.cpp",
    "ina3221_module.cpp",
    "rtc_pcf8563_module.cpp",
    "thing_speak.cpp",
    "debug_util.cpp",
    "WebServer_module.cpp",
    "eeprom_manager.cpp",
    "route_reboot.cpp",
    "wifi_sta_ap.cpp"};

//  --  setEnabled  --  //
void DebugUtil::setEnabled(bool enabled)
{
    debugEnabled = enabled;
}

//  --  isEnabled --  //
bool DebugUtil::isEnabled()
{
    return debugEnabled;
}

//  --  findModuleIndex  --  //
int DebugUtil::findModuleIndex(const char *moduleName)
{
    for (int i = 0; i < MAX_MODULES; i++)
    {
        if (modules[i].used && strcmp(modules[i].name, moduleName) == 0)
        {
            return i;
        }
    }
    return -1;
}

//  --  setModuleEnabled  --  //
void DebugUtil::setModuleEnabled(const char *moduleName, bool enabled)
{
    int idx = findModuleIndex(moduleName);
    if (idx >= 0)
    {
        modules[idx].enabled = enabled;
    }
    else
    {
        // Nieuwe module toevoegen
        for (int i = 0; i < MAX_MODULES; i++)
        {
            if (!modules[i].used)
            {
                strncpy(modules[i].name, moduleName, MAX_MODULE_NAME_LEN - 1);
                modules[i].name[MAX_MODULE_NAME_LEN - 1] = '\0';
                modules[i].enabled = enabled;
                modules[i].used = true;
                break;
            }
        }
    }
}

//  --  isModuleEnabled  --  //
bool DebugUtil::isModuleEnabled(const char *moduleName)
{
    int idx = findModuleIndex(moduleName);
    if (idx >= 0)
    {
        return modules[idx].enabled;
    }
    // Default: aan als niet gespecificeerd
    return true;
}

//  --  log  --  //
void DebugUtil::log(const char *filePath, const char *format, ...)
{
    if (!debugEnabled)
        return;

    const char *filename = strrchr(filePath, '/');
    if (!filename)
        filename = strrchr(filePath, '\\');
    filename = (filename) ? filename + 1 : filePath;

    if (!isModuleEnabled(filename))
        return;

    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    String finalLog = "[" + String(filename) + "] " + String(message);
    Serial.println(finalLog);

    // Voeg toe aan logbuffer
    logBuffer.push_back(finalLog);
    if (logBuffer.size() > MAX_LOG_LINES)
        logBuffer.erase(logBuffer.begin()); // verwijder oudste log
}

//  --  getLogBuffer  --  //
void DebugUtil::getLogBuffer(String &out)
{
    out = "";
    for (const String &line : logBuffer)
    {
        out += line + "\n";
    }
}

//  --  clearLogBuffer  --  //
void DebugUtil::clearLogBuffer()
{
    logBuffer.clear();
}

//  --  print  --  //
void DebugUtil::print(const char *filePath, const char *msg)
{
    if (!debugEnabled)
        return;

    const char *filename = strrchr(filePath, '/');
    if (!filename)
        filename = strrchr(filePath, '\\');
    filename = (filename) ? filename + 1 : filePath;

    if (!isModuleEnabled(filename))
        return;

    Serial.print(msg); // Zonder newline
}

// --  println  --  //
void DebugUtil::println(const char *filePath, const char *msg)
{
    if (!debugEnabled)
        return;

    const char *filename = strrchr(filePath, '/');
    if (!filename)
        filename = strrchr(filePath, '\\');
    filename = (filename) ? filename + 1 : filePath;

    if (!isModuleEnabled(filename))
        return;

    Serial.println(msg); // Met newline
}
void DebugUtil::initDefaultModules()
{
    setEnabled(true);
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        setModuleEnabled(moduleNames[i], true);
    }
}
