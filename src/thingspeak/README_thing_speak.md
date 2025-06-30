# ThingSpeak Module

Deze module verzorgt het versturen van sensor- en meetdata naar het ThingSpeak platform via HTTP.

---

## Functionaliteit

- Periodiek versturen van meetwaarden naar ThingSpeak op basis van een ingestelde interval.
- Ondersteunt het versturen van gegevens van verschillende sensoren (INA3221 kanaal 1 & 2, AHT20 + BMP280).
- Controleert WiFi-verbinding voordat data wordt verzonden.
- Logt uitgebreide debug-informatie over verzonden waarden en HTTP-responses.

---

## Gebruik

1. Initialiseer ThingSpeak in je `setup()`:
    ```cpp
    thingSpeak.begin();
    ```

2. Roep periodiek `update()` aan in je `loop()`, met het huidige tijdstip (millis):
    ```cpp
    void loop() {
        unsigned long now = millis();
        thingSpeak.update(now);
        // andere code...
    }
    ```

---

## Belangrijkste functies

- `void begin()`  
  Initialiseert de ThingSpeak module, bijvoorbeeld voor toekomstige configuratie.

- `void update(unsigned long now)`  
  Stuurt, als de ingestelde interval is verstreken, nieuwe data naar ThingSpeak.

- `void sendToThingSpeak()`  
  Bouwt de URL met sensordata en verstuurt deze via HTTP GET naar ThingSpeak.

---

## Vereisten

- WiFi verbinding via `WiFi.status()` (ESP32/ESP8266).
- HTTPClient bibliotheek.
- Toegang tot sensordata via singleton instanties van `INA3221_Module` en `AHT20BMP280_Module`.
- Parameters met ThingSpeak API sleutel (`Parameters::thingSpeak_ApiKey`).
- Timerwaarde `Timers::thingSpeak_UpdateInterval` voor update-interval.

---

## Bestanden

- `thing_speak.cpp` – Implementatie van ThingSpeak communicatie.  
- `thing_speak.h` – Declaraties en klasse definitie.

---

## Voorbeeld

```cpp
#include "thing_speak.h"

void setup() {
    Serial.begin(115200);
    thingSpeak.begin();
}

void loop() {
    unsigned long now = millis();
    thingSpeak.update(now);
    delay(1000);
}
