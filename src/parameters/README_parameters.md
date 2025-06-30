# Parameters Module

Deze module beheert de configuratieparameters van het project, inclusief timing-instellingen en netwerkgegevens.  
Parameters kunnen geladen worden vanuit een bestand op de SD-kaart, en er is een fallback naar standaardwaarden.  
Ook bevat het reflectie-achtige functies voor het dynamisch uitlezen en bijwerken van parameters.

---

## Functionaliteit

- Globale flags voor WiFi status en taal.
- Beheer van timerwaarden in de `Timers` namespace.
- Gevoelige en niet-gevoelige parameters in de `Parameters` namespace.
- Parameterreflectie via `ParameterDescriptor` struct.
- Laden en opslaan van parameters naar `/config/parameters.txt` op de SD-kaart.
- Parseren van key=value regels uit het configuratiebestand.
- Automatisch aanvullen van ontbrekende parameters in het bestand.
- Converteren van parameters naar een string map en andersom.
- Initialisatie met fallback op standaardwaarden.

---

## Gebruik

1. Initialiseer de parameters bij opstart:
    ```cpp
    Parameters::initParameters();
    ```

2. Lees alle parameters uit als map:
    ```cpp
    auto paramsMap = Parameters::getCurrentParametersAsMap();
    ```

3. Update parameters met een string map:
    ```cpp
    std::map<String, String> newSettings = { {"ap_ssid", "mijnssid"} };
    Parameters::updateParametersFromMap(newSettings);
    ```

---

## Structuren en namespaces

- `Timers` namespace met timing configuraties (alle unsigned long).  
- `Parameters` namespace met gevoelige en niet-gevoelige parameters (Strings en int).  
- `ParameterDescriptor` struct met key, pointer naar parameter en type (STRING, INT, LONG) voor reflectie.

---

## Belangrijkste functies

- `std::vector<ParameterDescriptor> getAllParameters()`  
  Retourneert alle parameters met metadata.

- `std::map<String, String> getCurrentParametersAsMap()`  
  Geeft een map van parameter-naam naar stringwaarde.

- `void initParameters()`  
  Laadt parameters van bestand of schrijft default bestand weg.

- `void updateParametersFromMap(const std::map<String, String> &settingsMap)`  
  Update de parameters aan de hand van een string map.

---

## Bestanden

- `parameters.cpp` – Implementatie van parameters en opslagbeheer.  
- `parameters.h` – Declaraties van parameters, structuren en API.

---

## Voorbeeld

```cpp
#include "parameters.h"

void setup() {
    Serial.begin(115200);

    // Initialiseer parameters
    initParameters();

    // Print alle parameters
    auto params = getCurrentParametersAsMap();
    for (const auto &p : params) {
        Serial.println(p.first + ": " + p.second);
    }

    // Pas parameter aan
    std::map<String, String> updates = { {"ap_ssid", "NieuweSSID"} };
    updateParametersFromMap(updates);
}
