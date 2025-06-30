# Logger Module

Deze module verzorgt het loggen van energiegegevens naar opslagmedia (SD-kaart of LittleFS) en het periodiek flushen van logbuffers.  
De logger werkt nauw samen met de SDCardUtils module en maakt gebruik van timing parameters voor intervalbeheer.

---

## Functionaliteit

- Initialisatie van het logging-systeem met voorkeur voor SD-kaart; valt terug op LittleFS indien SD niet beschikbaar is.
- Periodiek loggen van energiegegevens met een configureerbare interval.
- Automatisch flushen van logbuffers om dataverlies te voorkomen.
- Logging van statusmeldingen via DEBUG_LOG macro.

---

## Gebruik

1. Initialiseer de logger in je setup:
    ```cpp
    Logger::begin();
    ```

2. Roep periodiek `Logger::tick()` aan, bijvoorbeeld in je `loop()`, met de huidige milliseconden en tijd:
    ```cpp
    void loop() {
        unsigned long nowMillis = millis();
        DateTime now = rtc.now(); // Voorbeeld: gebruik RTC om actuele tijd te krijgen

        Logger::tick(nowMillis, now);

        // Andere logica
    }
    ```

---

## Belangrijkste functies

- `void begin()`  
  Start het logging systeem en initialiseert SDCardUtils. Fallback naar LittleFS bij falen.

- `void logEnergyIfNeeded(unsigned long millisNow, const DateTime &now)`  
  Logt energiegegevens als het ingestelde interval is verstreken.

- `void tick(unsigned long millisNow, const DateTime &now)`  
  Maakt gebruik van `logEnergyIfNeeded` en controleert of het tijd is om de logbuffer te flushen.

---

## Vereisten

- `SDCardUtils` module voor opslagbeheer.  
- RTC bibliotheek (zoals RTClib) voor tijdbeheer (`DateTime` object).  
- `Timers` struct/namespace met configuratievariabelen:
  - `Timers_energyLogInterval`  
  - `Timers_flushInterval`

- `DEBUG_LOG` macro voor debugging output.

---

## Bestanden

- `logger.cpp` - Implementatie van de logger functies.  
- `logger.h` - Header met functie declaraties.

---

## Voorbeeld

```cpp
#include "logger.h"
#include <RTClib.h>

void setup() {
    Serial.begin(115200);
    Logger::begin();
}

void loop() {
    unsigned long nowMillis = millis();
    DateTime now = rtc.now(); // Verondersteld dat rtc een globale RTC instantie is

    Logger::tick(nowMillis, now);

    delay(1000);
}
