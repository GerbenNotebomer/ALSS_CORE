#include "wifi_sta_ap.h"
#include "../common/common_includes.h"
#include "../hardware/rtc/rtc_pcf8563_module.h"

namespace WiFiManager
{
    void wipeNVS()
    {
        esp_err_t err = nvs_flash_erase();
        if (err == ESP_OK)
        {
            DEBUG_PRINTLN("✅ NVS gewist");
        }
        else
        {
            DEBUG_LOGF("❌ Fout bij wissen NVS: %s", esp_err_to_name(err));
        }
    }

    void startAccessPoint()
    {
        WiFi.softAPdisconnect(true); // Wis oude config
        delay(100);

        const char *ssid = Parameters::ap_ssid.c_str();
        const char *password = Parameters::ap_password.c_str();

        if (strlen(password) < 8)
            WiFi.softAP(ssid); // Open netwerk
        else
            WiFi.softAP(ssid, password);

        delay(200);
        DEBUG_LOGF("[WiFiManager] AP gestart: %s", ssid);
        DEBUG_LOGF("[WiFiManager] IP AP: %s", WiFi.softAPIP().toString().c_str());
    }
    bool connectToWiFi()
    {
        const int maxAttempts = 3;
        const unsigned long timeout = 10000;
        bool verbonden = false;

        for (int attempt = 1; attempt <= maxAttempts && !verbonden; ++attempt)
        {
            DEBUG_LOGF("[WiFiManager] Verbinden poging %d met WiFi1: %s", attempt, Parameters::wifi1_ssid.c_str());
            WiFi.begin(Parameters::wifi1_ssid.c_str(), Parameters::wifi1_password.c_str());

            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < timeout)
            {
                delay(500);
                DEBUG_PRINT(".");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                verbonden = true;
                break;
            }

            // Optioneel: proberen met WiFi2
            if (Parameters::wifi2_ssid.length() > 0)
            {
                DEBUG_LOGF("\n[WiFiManager] Verbinden poging %d met WiFi2: %s", attempt, Parameters::wifi2_ssid.c_str());
                WiFi.begin(Parameters::wifi2_ssid.c_str(), Parameters::wifi2_password.c_str());

                start = millis();
                while (WiFi.status() != WL_CONNECTED && millis() - start < timeout)
                {
                    delay(500);
                    DEBUG_PRINT(".");
                }

                if (WiFi.status() == WL_CONNECTED)
                {
                    verbonden = true;
                    break;
                }
            }
        }

        if (!verbonden)
        {
            wifiPermanentFailure = true; // ⛔️ Set flag: geen WiFi
        }

        return verbonden;
    }

    void printConnectedClients()
    {
        wifi_sta_list_t staList;
        tcpip_adapter_sta_list_t adapterList;

        esp_err_t result = esp_wifi_ap_get_sta_list(&staList);

        if (result != ESP_OK)
        {
            DEBUG_LOGF("❌ Kan lijst van verbonden clients niet ophalen: %s", esp_err_to_name(result));
            return;
        }

        result = tcpip_adapter_get_sta_list(&staList, &adapterList);

        if (result != ESP_OK)
        {
            DEBUG_LOGF("❌ Kan IP-adressen van clients niet ophalen: %s", esp_err_to_name(result));
            return;
        }

        DEBUG_LOGF("🔗 Verbonden clients: %d", adapterList.num);

        for (int i = 0; i < adapterList.num; i++)
        {
            tcpip_adapter_sta_info_t info = adapterList.sta[i];

            // IP adres naar string in buffer
            char ipStr[16];
            snprintf(ipStr, sizeof(ipStr), IPSTR, IP2STR(&info.ip));

            // Log naar jouw debug systeem
            DEBUG_LOGF("📱 Client %d -> MAC: %02X:%02X:%02X:%02X:%02X:%02X | IP: %s",
                       i + 1,
                       info.mac[0], info.mac[1], info.mac[2],
                       info.mac[3], info.mac[4], info.mac[5],
                       ipStr);
        }
    }

    void begin()
    {
        // Initialiseer NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            nvs_flash_erase();
            nvs_flash_init();
        }

        WiFi.mode(WIFI_MODE_NULL);
        delay(100);
        WiFi.mode(WIFI_AP_STA);

        startAccessPoint(); // Start eigen AP

        bool success = connectToWiFi();
        if (success)
        {
            DEBUG_LOGF("\n[WiFiManager] Verbonden met: %s", WiFi.SSID().c_str());
            DEBUG_LOGF("[WiFiManager] IP STA: %s", WiFi.localIP().toString().c_str());
            rtcManager.syncTimeWithNTP();
        }
        else
        {
            DEBUG_LOGF("\n[WiFiManager] Verbinden met WiFi mislukt");
        }
    }

    void checkReconnect()
    {
        static unsigned long lastReconnectAttempt = 0;

        if (wifiPermanentFailure)
            return; // ❌ Niet opnieuw proberen

        if (WiFi.status() == WL_CONNECTED)
            return;

        if (millis() - lastReconnectAttempt < Timers::Timers_wifiReconnectInterval)
            return;

        lastReconnectAttempt = millis();
        DEBUG_PRINTLN("[WiFiManager] Verbindingsverlies gedetecteerd. Herverbinden...");

        WiFi.disconnect(true); // Forceer disconnect
        delay(100);

        bool success = connectToWiFi();
        if (success)
        {
            DEBUG_LOGF("\n[WiFiManager] Herverbonden met: %s", WiFi.SSID().c_str());
            DEBUG_LOGF("[WiFiManager] Nieuw IP-adres: %s", WiFi.localIP().toString().c_str());
        }
        else
        {
            DEBUG_PRINTLN("\n[WiFiManager] Herverbinden mislukt.");
        }
    }

}
