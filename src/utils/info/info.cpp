#include "info.h"
#include <WiFi.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp32/rom/rtc.h>
#include "../../hardware/sd_card/sd_card_module.h"
#include "../../common/common_includes.h"

extern bool parametersFromFile; // extern vlag vanuit parameters module
// extern class LangManager langManager; // extern taalmanager, pas aan als nodig

String getBoardName()
{
    return F("ESP32-S2 Mini");
}

String getSystemInfo()
{
    String info;
    info.reserve(512);

    info += F("Vrij geheugen: ");
    info += String(esp_get_free_heap_size() / 1024.0, 2);
    info += F(" KB<br>");

    info += F("Totaal geheugen: ");
    info += String(ESP.getHeapSize() / 1024.0, 2);
    info += F(" KB<br>");

    uint32_t uptime = millis() / 1000;
    info += F("Uptime: ");
    info += String(uptime / 3600) + F("h ") +
            String((uptime % 3600) / 60) + F("m ") +
            String(uptime % 60) + F("s<br>");

    info += F("CPU frequentie: ");
    info += String(ESP.getCpuFreqMHz());
    info += F(" MHz<br>");

    size_t flashSize = 0;
    esp_flash_get_size(NULL, &flashSize);
    info += F("Flash grootte: ");
    info += String(flashSize / (1024 * 1024));
    info += F(" MB<br>");

    bool psramAvailable = esp_spiram_is_initialized();
    info += F("PSRAM beschikbaar: ");
    info += (psramAvailable ? F("Ja") : F("Nee"));
    info += F("<br>");

    if (psramAvailable)
    {
        info += F("PSRAM vrij: ");
        info += String(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024.0, 2);
        info += F(" KB<br>");
    }

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    info += F("MAC-adres: ");
    info += String(macStr);
    info += F("<br>");

    info += F("IP adres: ");
    info += WiFi.localIP().toString();
    info += F("<br>");

    info += F("Signaalsterkte: ");
    info += String(WiFi.RSSI());
    info += F(" dBm<br>");

    info += F("SSID: ");
    info += WiFi.SSID();
    info += F("<br>");

    info += F("SD-kaart aanwezig: ");
    info += (SD.begin(10) ? F("✅ Ja") : F("❌ Nee"));
    info += F("<br>");

    info += F("Parameters geladen uit: ");
    info += (parametersFromFile ? F("📄 parameters.txt") : F("🛠️ standaardwaarden"));
    info += F("<br>");

    info += F("Taal: ");
    info += langManager.getCurrentLanguage();
    info += F("<br>");

    return info;
}

void checkPsram()
{
    if (psramFound())
        DEBUG_LOGF("✅ PSRAM actief: %d KB", ESP.getPsramSize() / 1024);
    else
        DEBUG_LOG("❌ Geen PSRAM actief");
}
