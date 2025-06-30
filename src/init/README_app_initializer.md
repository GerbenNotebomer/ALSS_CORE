# AppInitializer (Applicatie Initialisatie)

## Overzicht
Deze module verzorgt de initiële setup en configuratie van de applicatie.  
Het initialiseert hardwarecomponenten, communicatieprotocollen, logging, taalinstellingen, en netwerkdiensten.

---

## Bestand: app_initializer.cpp

```cpp
#include <Arduino.h>
#include <Wire.h>

#include "app_initializer.h"
#include "../common/common_includes.h"
#include "../logger/logger.h"
#include "../utils/debug/debug_util.h"
#include "../utils/eeprom/eeprom_manager.h"
#include "../hardware/aht20_bmp280/aht20_bmp280_module.h"
#include "../hardware/ina3221/ina3221_module.h"
#include "../hardware/rtc/rtc_pcf8563_module.h"
#include "../parameters/parameters.h"
#include "../language/language_manager.h"
#include "../thingspeak/thing_speak.h"
#include "../wifi_sta_ap/wifi_sta_ap.h"
#include "../webserver/web_server_manager.h"
#include "../webserver/cache_manager.h"

//  --  begin  --  //
void AppInitializer::begin()
{
    Serial.begin(115200);
    delay(1000);
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!SDCardUtils::begin(10))
    {
        DEBUG_LOG("⚠️ Logging valt terug op LittleFS.");
    }

    Logger::begin();

    if (!langManager.init())
    {
        DEBUG_LOG("❌ Taal initialisatie mislukt");
    }

    EEPROM_Manager::loadDefaultsIfNeeded();
    DebugUtil::initDefaultModules();
    EEPROM_Manager::init();

    checkPsram(); // Dit zit in utils/info/info.cpp

    AHT20BMP280_Module::getInstance().begin();
    rtcManager.begin();
    thingSpeak.begin();
    INA3221_Module::getInstance().begin();

    initParameters();
    WiFiManager::begin();
    WebServerManager::begin();
}
Toelichting & Tips  
Serial communicatie: Initialisatie met baudrate 115200 en een korte delay voor stabilisatie.

I2C-bus: Wordt gestart met gedefinieerde SDA en SCL pinnen (I2C_SDA, I2C_SCL).

Opslagbeheer: Probeert SD-kaart initialisatie via CS-pin 10; valt terug op LittleFS als SD faalt.

Logging: Logger module wordt gestart voor het bijhouden van runtime data.

Taalbeheer: langManager initialiseert de taalbestanden; fouten worden gelogd.

EEPROM: Laadt standaardwaarden indien nodig, initialiseert EEPROM en debug modules.

Hardware modules: Initialisatie van sensoren (AHT20/BMP280), RTC (PCF8563), energiemeting (INA3221).

Parameters en netwerk: Parameter initialisatie, WiFi management (station + access point modus), en webserver start.

Debugging: Gebruik van DEBUG_LOG macro voor het loggen van fouten en statusmeldingen.

PSRAM: Extra geheugencontrole via checkPsram() (in utils/info/info.cpp).

Structuur: Zorg dat afhankelijkheden (zoals rtcManager, langManager, SDCardUtils, etc.) globaal beschikbaar en correct geïmporteerd zijn.

Gebruik  
Roep AppInitializer::begin(); aan in je setup() functie om de volledige applicatie setup te starten:

#include "app_initializer.h"

void setup() {
    AppInitializer::begin();
}

void loop() {
    // Je hoofdlus code hier
}

Bestanden  
app_initializer.cpp  
Implementatie van de begin-functie met volledige initialisatie van hardware en software modules.

app_initializer.h  
Header met de declaratie van de begin-functie.

Overige dependencies: rtc_pcf8563_module.h, logger.h, language_manager.h, wifi_sta_ap.h, etc.