# SDCardUtils (SD-kaart en LittleFS beheer)

## Overzicht
Deze module verzorgt het beheer van SD-kaart en LittleFS-bestandssystemen voor het opslaan en uitlezen van bestanden.  
Het bevat functies voor initialisatie, bestandsmanipulatie, directory listings en het loggen van energiewaarden.

---

## Class: SDCardUtils

### Belangrijkste functies

- `begin(uint8_t csPin = 10)`  
  Initialiseert de SD-kaart via SPI op opgegeven CS-pin.  
  Probeert maximaal 5 keer te verbinden met de SD-kaart.  
  Bij falen schakelt fallback naar LittleFS.  
  Geeft true terug bij succes, anders false.

- `isUsingSD()`  
  Retourneert `true` als SD-kaart wordt gebruikt, `false` als LittleFS.

- `fileExists(const String &path)`  
  Controleert of een bestand bestaat op SD of LittleFS.

- `writeFile(const String &path, const String &content, bool append = false)`  
  Schrijft (of voegt toe aan) een bestand op SD of LittleFS.

- `readFile(const String &path)`  
  Leest de inhoud van een bestand uit.

- `deleteFile(const String &path)`  
  Verwijdert een bestand indien het bestaat.

- `getDirectoryListing(const String &dir, uint8_t levels = 0)`  
  Geeft een overzicht van bestanden en mappen, recursief tot het opgegeven niveau.

- `logEnergyToSD(const DateTime &now)`  
  Logt energiewaarden (via INA3221 module) naar een CSV-bestand.  
  Maakt de logmap aan indien nodig.  
  Houdt een buffer bij en schrijft die periodiek weg.

- `flushLogBuffer()`  
  Schrijft de buffer met logregels direct naar het bestand.

- `formatTwoDigits(int val)`  
  Geeft een getal terug als string, met voorloopnul indien < 10.

- `getLogFilePath(const DateTime &dt)`  
  Bepaalt het logbestandspad op basis van de datum.

- `ensureLogDirectoriesExist()`  
  Zorgt dat de /log map aanwezig is op SD of LittleFS.

- `appendToFile(const char *path, const String &data)`  
  Voegt data toe aan een bestand op SD.

---

## Variabelen en Members

- `logBuffer`  
  Buffer voor tijdelijk opslaan van logregels.

- `useSD`  
  Boolean die aangeeft of SD-kaart actief is (anders LittleFS).

- `lastLogFile`  
  Pad naar het laatste logbestand waarnaar wordt geschreven.

---

## Voorbeeldgebruik

#include "sd_card_module.h"

void setup() {
    Serial.begin(115200);
    if (SDCardUtils::begin(5)) {
        Serial.println("SD-kaart of LittleFS gestart");
    } else {
        Serial.println("Initialisatie mislukt");
    }
}

void loop() {
    DateTime now = rtcManager.now();
    SDCardUtils::logEnergyToSD(now);
    delay(60000); // iedere minuut loggen
}

---
