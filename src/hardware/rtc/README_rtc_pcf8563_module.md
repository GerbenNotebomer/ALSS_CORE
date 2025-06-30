# RTCManager (RTC PCF8563 Module)

## Overzicht
Deze module verzorgt het beheer van een **PCF8563 Real Time Clock (RTC)** module.  
Het biedt functionaliteiten voor het starten van de RTC, synchronisatie met NTP-servers, en het ophalen van geformatteerde datum- en tijdgegevens.

---

## Class: RTCManager

### Belangrijkste functies

- `begin()`
  - Initialiseert de PCF8563 RTC module.
  - Logt of de RTC succesvol is gevonden en gestart.

- `syncTimeWithNTP()`
  - Synchroniseert de RTC tijd met behulp van NTP-servers (`pool.ntp.org`, `time.nist.gov`).
  - Stelt de tijd in op basis van de ontvangen NTP-tijd.
  - Logt het resultaat van de synchronisatie.

- `getFormattedDateTime()`
  - Retourneert een string met de huidige datum en tijd in het formaat `DD/MM/YYYY HH:MM:SS`.
  - Zorgt voor voorloopnullen bij waarden kleiner dan 10 voor nette weergave.

- `getDag()`
  - Bepaalt de dag van de week als een gelokaliseerde string via een taalmanager.
  - Gebruik maakt van indices 0-6 (zondag t/m zaterdag).

- `now()`
  - Retourneert een `DateTime` object van de huidige tijd uit de RTC.

---

## Variabelen en Members

- `rtc`  
  Instantie van de `RTC_PCF8563` uit de RTClib bibliotheek, die hardware-interactie verzorgt.

- `rtcManager`  
  Globale instantie van `RTCManager` voor eenvoudige toegang in de applicatie.

---

## Voorbeeldgebruik

#include "rtc_pcf8563_module.h"

void setup() {
    Serial.begin(115200);
    rtcManager.begin();

    // Synchroniseer tijd met NTP-server
    rtcManager.syncTimeWithNTP();
}

void loop() {
    String dateTime = rtcManager.getFormattedDateTime();
    String dayName = rtcManager.getDag();

    Serial.println("Datum en tijd: " + dateTime);
    Serial.println("Vandaag is: " + dayName);

    delay(1000);
}

---

## Toelichting & Tips
NTP Synchronisatie: De functie syncTimeWithNTP() gebruikt de ESP32/ESP8266 functie configTime() om via internet de juiste tijd op te halen. Zorg dat het apparaat verbonden is met WiFi voordat je deze functie aanroept.

Tijdzone: In configTime() wordt het tijdsverschil ingesteld op +3600 seconden (1 uur) voor zowel standaardtijd als zomertijd. Pas dit aan indien nodig.

Lokalisatie: De dagnaam wordt opgehaald via langManager.get(), zorg dat je taalbestanden of vertalingen beschikbaar hebt voor de keys dagen.zondag t/m dagen.zaterdag.

DateTime object: De RTClib DateTime class biedt handige methoden voor datum- en tijdmanipulatie.

Debugging: Gebruik de DEBUG_LOG macro voor het loggen van statusmeldingen tijdens ontwikkeling.

---

## Bestanden
rtc_pcf8563_module.h
Bevat de classdefinitie van RTCManager.

rtc_pcf8563_module.cpp
Bevat de implementatie van alle methoden van RTCManager.
