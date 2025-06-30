// Wifi_STA_AP.h
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h" // voor IPSTR en IP2STR

namespace WiFiManager
{
    void begin();          // Start AP en probeer STA
    void checkReconnect(); // Periodieke check op STA-verbinding
    void wipeNVS();        // Optioneel: geheugen wissen
    void printConnectedClients();
}
