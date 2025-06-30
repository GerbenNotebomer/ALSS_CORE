#pragma once
#include <Arduino.h>
#include <vector>
#include <stdarg.h>
#include <string.h>

#define MAX_MODULES 14

#define MAX_MODULE_NAME_LEN 32

const int MAX_DEBUG_MODULES = 14;
extern const char *moduleNames[MAX_DEBUG_MODULES];

class DebugUtil
{
public:
    static void setEnabled(bool enabled);
    static bool isEnabled();
    static void setModuleEnabled(const char *moduleName, bool enabled);
    static bool isModuleEnabled(const char *moduleName);
    static void log(const char *filePath, const char *format, ...);
    static std::vector<String> logBuffer;
    static void getLogBuffer(String &out);
    static void clearLogBuffer();
    static void print(const char *filePath, const char *msg);
    static void println(const char *filePath, const char *msg);
    static void initDefaultModules();

private:
    static bool debugEnabled;

    static const size_t MAX_LOG_LINES;
    struct ModuleDebug
    {
        char name[MAX_MODULE_NAME_LEN];
        bool enabled;
        bool used;
    };
    static ModuleDebug modules[MAX_MODULES];

    static int findModuleIndex(const char *moduleName);
};

#define DEBUG_LOGF(fmt, ...) DebugUtil::log(__FILE__, fmt, ##__VA_ARGS__)
#define DEBUG_LOG(msg) DebugUtil::log(__FILE__, "%s", msg)
#define DEBUG_PRINT(msg) DebugUtil::print(__FILE__, msg)
#define DEBUG_PRINTLN(msg) DebugUtil::println(__FILE__, msg)
