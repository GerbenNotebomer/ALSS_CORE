# LoopTasks Module

Deze module beheert verschillende periodieke taken die tijdens de hoofdlus van de applicatie uitgevoerd moeten worden.  
Het zorgt voor het updaten van hardware modules, het loggen, netwerkbeheer en caching.

---

## Functionaliteit

- Periodieke updates van sensoren en logging volgens een configureerbare interval.
- Flushen van logbuffers op vaste tijdsintervallen.
- Aanroepen van cachebeheer via een dynamisch toegekende functie.
- Periodieke communicatie met ThingSpeak voor data upload.
- Beheer van WiFi connectiviteit en automatische reconnect.
- Periodieke debug-output van verbonden WiFi-clients.

---

## Gebruik

1. Stel een cache manager in (optioneel):
    ```cpp
    LoopTasks::setCache(cacheManagerPointer);
    ```

2. Roep periodiek de taken aan in je `loop()` functie:
    ```cpp
    void loop() {
        LoopTasks::handlePeriodicUpdates();
        LoopTasks::handleThingSpeak();
        LoopTasks::handleWifi();
        LoopTasks::printDebugEveryMinute();

        // Andere logica
    }
    ```

---

## Belangrijkste functies

- `setCache(void *cache)`  
  Koppelt een `CacheManager` instantie die periodiek getickt wordt.

- `handlePeriodicUpdates()`  
  Voert updates uit voor sensoren, logging en flushes op basis van timers.

- `handleThingSpeak()`  
  Beheert de data upload naar ThingSpeak, mits WiFi niet permanent uitgevallen is.

- `handleWifi()`  
  Controleert en onderhoudt de WiFi verbinding, met reconnect logica.

- `printDebugEveryMinute()`  
  Print elke minuut debug informatie over verbonden clients.

---

## Vereisten

- `Timers` struct/namespace met configuratievariabelen:
  - `Timers_updateInterval`  
  - `Timers_flushInterval`

- Globale instanties / modules:
  - `rtcManager` (voor tijd)
  - `INA3221_Module` (voor sensor updates)
  - `Logger`
  - `SDCardUtils`
  - `thingSpeak`
  - `WiFiManager`
  - `CacheManager` (optioneel)

- Globale variabele `wifiPermanentFailure` voor WiFi status.

---

## Bestanden

- `loop_tasks.cpp` - Implementatie van de loop taken.  
- `loop_tasks.h` - Header met functie declaraties.

---

## Voorbeeld

```cpp
#include "loop_tasks.h"

void setup() {
    // Setup code hier
}

void loop() {
    LoopTasks::handlePeriodicUpdates();
    LoopTasks::handleThingSpeak();
    LoopTasks::handleWifi();
    LoopTasks::printDebugEveryMinute();

    delay(10);
}
