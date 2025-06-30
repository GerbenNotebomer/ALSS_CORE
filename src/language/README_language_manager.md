# LanguageManager

Deze module verzorgt het beheer van taalinstellingen en het laden van vertalingen vanuit JSON-bestanden.  
De module maakt gebruik van LittleFS voor bestandstoegang en EEPROM om de voorkeurstaal op te slaan.

---

## Functionaliteit

- Instellen van de huidige taal met fallback naar Nederlands ("nl").
- Laden van vertalingen vanuit een JSON-bestand.
- Ophalen van vertalingen op basis van een sleutel.
- Ophalen van de lijst met beschikbare talen uit het geladen JSON-document.
- Initialisatie van de module met automatische mount van LittleFS en laden van opgeslagen taalinstelling uit EEPROM.

---

## Gebruik

1. Maak een instantie van `LanguageManager` aan (meestal globaal):
    ```cpp
    LanguageManager langManager;
    ```

2. Initialiseer in `setup()` met het pad naar het standaard taalbestand:
    ```cpp
    void setup() {
        if (!langManager.init("/languages/nl.json")) {
            // Fout afhandeling
        }
    }
    ```

3. Haal vertalingen op via de `get()` methode:
    ```cpp
    String text = langManager.get("welcome_message");
    ```

4. Optioneel: wissel taal via `setLanguage()` en laad andere vertalingen.

---

## Belangrijkste methoden

- `bool init(const String &defaultLangPath)`  
  Mount LittleFS, laadt de taalcode uit EEPROM, en laadt vertalingen uit het JSON bestand.

- `void setLanguage(const String &lang)`  
  Stel de actieve taal in.

- `String get(const String &key) const`  
  Haal de vertaling op voor een sleutel, met fallback.

- `std::vector<String> getAvailableLanguages() const`  
  Krijg een lijst van beschikbare talen in het JSON bestand.

- `String getCurrentLanguage() const`  
  Krijg de huidige actieve taal.

---

## Vereisten

- ArduinoJson library  
- LittleFS bestandssysteem  
- EEPROM_Manager voor opslag van taalcode  

---

## Bestanden

- `language_manager.cpp` - Implementatie van de module.  
- `language_manager.h` - Header met klasse-definitie.

---

## Opmerkingen

- JSON-bestanden moeten het formaat hebben waarbij iedere sleutel een object met taalcodes en vertalingen bevat, bijvoorbeeld:
  ```json
  {
    "welcome_message": {
      "nl": "Welkom",
      "en": "Welcome"
    }
  }

De module maakt gebruik van DEBUG_LOG macro’s voor logging.

#include "language_manager.h"

LanguageManager langManager;

void setup() {
    Serial.begin(115200);
    if (!langManager.init("/languages/nl.json")) {
        Serial.println("Taal initialisatie mislukt");
    }
}

void loop() {
    Serial.println(langManager.get("welcome_message"));
    delay(5000);
}
