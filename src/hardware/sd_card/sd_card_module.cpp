#include "sd_card_module.h"
#include "../../utils/debug/debug_util.h"
#include "hardware/ina3221/ina3221_module.h"

#include <vector>
#include <FS.h>
#include <SD.h>
#include <LittleFS.h>
#include <SPI.h>

#define LOG_BUFFER_SIZE 50

static std::vector<String> logBuffer;
static bool useSD = true;
static String lastLogFile = "";

////  ----  SDCardUtils ----  ////
//  --  begin  --  //
bool SDCardUtils::begin(uint8_t csPin)
{
    const int maxAttempts = 5;
    int attempt = 0;

    SPI.end();
    delay(100);
    SPI.begin(12 /* SCK */, 13 /* MISO */, 11 /* MOSI */, csPin);
    delay(100);

    while (attempt < maxAttempts)
    {
        attempt++;
        DEBUG_LOGF("[begin] 🔁 Poging %d om SD-kaart te initialiseren...", attempt);

        if (SD.begin(csPin) && SD.cardType() != CARD_NONE)
        {
            DEBUG_LOG("[begin] ✅ SD-kaart beschikbaar");
            useSD = true;
            return true;
        }

        DEBUG_LOGF("[begin] ❌ Poging %d mislukt. SD.begin() faalde.", attempt);
        SD.end();
        delay(200);
    }

    DEBUG_LOG("[begin] ⚠️ SD-kaart niet beschikbaar, fallback op LittleFS");
    if (!LittleFS.begin())
    {
        DEBUG_LOG("[begin] ❌ LittleFS initialisatie mislukt.");
        return false;
    }

    useSD = false;
    return true;
}

//  --  fileExists  --  //
bool SDCardUtils::fileExists(const String &path)
{
    return useSD ? SD.exists(path) : LittleFS.exists(path);
}

//  --  writeFile  --  //
bool SDCardUtils::writeFile(const String &path, const String &content, bool append)
{
    File file = useSD ? SD.open(path, append ? FILE_APPEND : FILE_WRITE)
                      : LittleFS.open(path, append ? FILE_APPEND : FILE_WRITE);

    if (!file)
    {
        DEBUG_LOGF("[writeFile] ❌ Kan bestand niet openen: %s", path.c_str());
        return false;
    }

    file.print(content);
    file.close();
    DEBUG_LOGF("[writeFile] 📄 Bestand %s %s", path.c_str(), append ? "aangevuld" : "overschreven");
    return true;
}

//  --  readFile  --  //
String SDCardUtils::readFile(const String &path)
{
    File file = useSD ? SD.open(path) : LittleFS.open(path);
    if (!file)
    {
        DEBUG_LOGF("[readFile] ❌ Kan bestand niet openen: %s", path.c_str());
        return "";
    }

    String content;
    while (file.available())
        content += (char)file.read();

    file.close();
    DEBUG_LOGF("[readFile] 📄 Bestand gelezen: %s (%d bytes)", path.c_str(), content.length());
    return content;
}

//  --  deleteFile  --  //
bool SDCardUtils::deleteFile(const String &path)
{
    if (!fileExists(path))
    {
        DEBUG_LOGF("[deleteFile] ⚠️ Bestand bestaat niet: %s", path.c_str());
        return false;
    }

    bool success = useSD ? SD.remove(path) : LittleFS.remove(path);
    DEBUG_LOGF("[deleteFile] %s %s", success ? "🗑️ Verwijderd" : "❌ Verwijderen mislukt", path.c_str());
    return success;
}

//  --  getDirectoryListing  --  //
String SDCardUtils::getDirectoryListing(const String &dir, uint8_t levels)
{
    File root = useSD ? SD.open(dir) : LittleFS.open(dir);
    if (!root || !root.isDirectory())
        return "❌ Geen geldige map: " + dir + "<br>";

    String result;
    File file = root.openNextFile();
    while (file)
    {
        String fullPath = dir;
        if (!fullPath.endsWith("/"))
            fullPath += "/";
        fullPath += file.name();

        if (file.isDirectory())
        {
            result += "📁 " + fullPath + "/<br>";
            if (levels > 0)
                result += getDirectoryListing(fullPath, levels - 1);
        }
        else
        {
            result += "📄 " + fullPath + " (" + String(file.size()) + " bytes)<br>";
        }

        file = root.openNextFile();
    }
    return result;
}

//  --  formatTwoDigits  --  //
String SDCardUtils::formatTwoDigits(int val)
{
    return (val < 10) ? "0" + String(val) : String(val);
}

//  --  getLogFilePath  --  //
String SDCardUtils::getLogFilePath(const DateTime &dt)
{
    String filename = "/log/log_" +
                      String(dt.year()) + "-" +
                      formatTwoDigits(dt.month()) + "-" +
                      formatTwoDigits(dt.day()) + ".csv";
    lastLogFile = filename;
    return filename;
}

//  --  flushLogBuffer  --  //
void SDCardUtils::flushLogBuffer()
{
    if (logBuffer.empty())
        return;

    File file = useSD ? SD.open(lastLogFile, FILE_APPEND)
                      : LittleFS.open(lastLogFile, FILE_APPEND);

    if (!file)
    {
        DEBUG_LOGF("[flushLogBuffer] ❌ Kan logbestand niet openen: %s", lastLogFile.c_str());
        return;
    }

    for (const String &line : logBuffer)
    {
        file.print(line);
    }

    file.close();
    DEBUG_LOGF("[flushLogBuffer] ✅ Logbuffer weggeschreven (%d regels)", logBuffer.size());
    logBuffer.clear();
}

//  --  logEnergyToSD  --  //
void SDCardUtils::logEnergyToSD(const DateTime &now)
{
    // Zorg dat map /log bestaat
    ensureLogDirectoriesExist();

    // Zorg dat lastLogFile correct staat (belangrijk voor flushLogBuffer)
    lastLogFile = getLogFilePath(now);

    String timestamp = String(now.year()) + "-" + formatTwoDigits(now.month()) + "-" +
                       formatTwoDigits(now.day()) + " " + formatTwoDigits(now.hour()) + ":" +
                       formatTwoDigits(now.minute());

    String line = timestamp;

    // Haal de channels op via INA3221_Module singleton
    auto &channels = INA3221_Module::getInstance().channels;

    for (int i = 1; i <= NUM_INA_CHANNELS; i++)
    {
        const ChannelData &ch = channels[i]; // Let op of channels 1-based is, of 0-based
        line += "," + String(ch.totalAh, 3) +
                "," + String(ch.totalWh, 3) +
                "," + String(ch.filteredBusVoltage, 2) +
                "," + String(ch.filteredCurrent_A, 2);
    }

    line += "\n";

    if (logBuffer.empty() && !fileExists(lastLogFile))
    {
        String header = "Tijdstip";
        for (int i = 1; i <= NUM_INA_CHANNELS; i++)
        {
            header += ",CH" + String(i) + "_Ah,CH" + String(i) + "_Wh,CH" + String(i) + "_V,CH" + String(i) + "_A";
        }
        header += "\n";
        logBuffer.push_back(header);
    }

    logBuffer.push_back(line);

    if (logBuffer.size() >= LOG_BUFFER_SIZE)
    {
        flushLogBuffer();
    }

    DEBUG_LOGF("[logEnergyToSD] 📝 Gebufferd: %s", line.c_str());
}

//  --  ensureLogDirectoriesExist  --  //
void SDCardUtils::ensureLogDirectoriesExist()
{
    if (!fileExists("/log"))
    {
        if (useSD)
        {
            SD.mkdir("/log");
            DEBUG_LOG("[ensureLogDirectoriesExist] 📁 /log aangemaakt op SD");
        }
        else
        {
            LittleFS.mkdir("/log");
            DEBUG_LOG("[ensureLogDirectoriesExist] 📁 /log aangemaakt op LittleFS");
        }
    }
}

//  --  isUsingSD  --  //
bool SDCardUtils::isUsingSD()
{
    return useSD;
}

//  --  appendToFile  --  //
bool SDCardUtils::appendToFile(const char *path, const String &data)
{
    File file = SD.open(path, FILE_APPEND);
    if (!file)
        return false;

    file.print(data);
    file.close();
    return true;
}
