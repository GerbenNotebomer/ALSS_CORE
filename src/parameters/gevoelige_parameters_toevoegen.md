# Gevoelige Parameters (`parameters_private.cpp`)

In dit project worden gevoelige instellingen zoals WiFi-wachtwoorden en API-sleutels **niet opgeslagen in versiebeheer** (zoals Git). In plaats daarvan moeten deze door de gebruiker handmatig worden toegevoegd in een apart bestand genaamd:

```
src/config/parameters_private.cpp
```

## ✏️ Wat moet je doen?

1. **Maak het bestand aan** als het nog niet bestaat:
   ```
   src/config/parameters_private.cpp
   ```

2. **Vul het bestand met je persoonlijke instellingen.** Hieronder staat een voorbeeldstructuur:

```cpp
#include "parameters.h"

// --- Privé/standaard waarden voor parameters ---

// Parameters Namespace
namespace Parameters
{
    // WiFi Access Point
    String ap_ssid = "ALSS_AP";
    String ap_password = "123456789";

    // WiFi netwerken
    String wifi1_ssid = "JouwWiFiNaam1";
    String wifi1_password = "JouwWachtwoord1";
    String wifi2_ssid = "JouwWiFiNaam2";
    String wifi2_password = "JouwWachtwoord2";

    // ThingSpeak instellingen
    String thingSpeak_Host = "api.thingspeak.com";
    String thingSpeak_ApiKey = "JouwAPIKeyHier";

}

// Timers Namespace
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

// Algemene flags
bool wifiPermanentFailure = false;
String currentLanguage = "nl";
bool parametersFromFile = false;
```

> 💡 **Let op:** Dit bestand bevat gevoelige informatie en hoort **NIET** in versiebeheer (zoals Git) te staan. Zorg dat je `.gitignore` dit bestand uitsluit.

## 📂 Best practices

- **Check `.gitignore`:** Voeg toe:
  ```
  src/config/parameters_private.cpp
  ```

- **Gebruik alleen lokaal:** Dit bestand is bedoeld om lokale geheimen en wachtwoorden in te bewaren. Deel dit bestand **nooit** publiekelijk.

---

Met dit systeem kunnen alle gebruikers hun eigen netwerk- en API-instellingen invoeren zonder dat deze gegevens in de gedeelde codebase terechtkomen.