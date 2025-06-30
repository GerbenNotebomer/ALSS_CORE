# EEPROM_Manager Module

Deze module verzorgt het opslaan en laden van diverse instellingen en kalibratiegegevens in de EEPROM van een ESP32/ESP8266.

---

## Functionaliteit

- Beheer van EEPROM-versie en compatibiliteit.
- Opslaan en laden van:
  - Kalibratiegegevens van INA3221 sensormodule.
  - Accu Ampère-uren (Ah) waarden.
  - Batterijcapaciteit (Ah).
  - Debug-module instellingen (aan/uit per module).
  - Taalcode voor de interface.
- Factory reset functie die EEPROM wist en standaardwaarden herstelt.
- Automatisch laden van standaardwaarden bij een ongeldige EEPROM-versie.

---

## Gebruik

### Initialisatie

Roep bij het opstarten aan:

```cpp
EEPROM_Manager::init();
Dit laadt alle opgeslagen instellingen uit EEPROM.

Factory reset
Wil je de EEPROM helemaal wissen en terugzetten naar standaardwaarden:


EEPROM_Manager::factoryResetEEPROM();
Debug instellingen opslaan en laden
De debug instellingen worden automatisch geladen en opgeslagen via:


EEPROM_Manager::loadDebugSettingsFromEEPROM();
EEPROM_Manager::saveDebugSettingsToEEPROM();
Kalibratie opslaan en laden

EEPROM_Manager::loadCalibrationFromEEPROM();
EEPROM_Manager::saveCalibrationToEEPROM();
Accu-gegevens opslaan en laden

EEPROM_Manager::loadAhFromEEPROM();
EEPROM_Manager::saveAhToEEPROM();

EEPROM_Manager::loadCapacityFromEEPROM();
EEPROM_Manager::saveCapacityToEEPROM();
Taalcode opslaan en laden

EEPROM_Manager::saveLanguageToEEPROM("nl");
String lang = EEPROM_Manager::loadLanguageFromEEPROM();
Structuur
EEPROM_Manager is volledig statisch, dus je hoeft geen object aan te maken.

debugSettings houdt bij welke debug modules aan of uit staan.

EEPROM-adressen zijn gedefinieerd als constexpr constants.

Module maakt gebruik van de singleton INA3221_Module voor kalibratie en accu-data.

Voorbeeld

#include "eeprom_manager.h"

void setup() {
    Serial.begin(115200);
    EEPROM_Manager::init();

    if (!EEPROM_Manager::isEEPROMVersionValid()) {
        EEPROM_Manager::loadDefaultsIfNeeded();
    }
}
Afhankelijkheden
Arduino.h

EEPROM.h

debug_util.h (voor debug logging)

INA3221_Module hardware interface