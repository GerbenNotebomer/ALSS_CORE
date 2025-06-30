# Hoofdprogramma ALSS_V2.1

## Overzicht
Dit is de hoofdcode van het ALSS project, die de Arduino setup en loop regelt. 

- `setup()` initialiseert alle subsystemen zoals WiFi, hardware, logging en webserver functionaliteit.
- Een `CacheManager` wordt opgezet om JSON data te cachen die door de webserver gebruikt wordt.
- De loop verwerkt periodieke taken zoals het uitlezen van sensoren, het versturen van data naar ThingSpeak, en het beheren van WiFi connecties.
- Debug informatie wordt elke minuut uitgeprint.

## Belangrijke modules
- `AppInitializer` (init): zorgt voor de startconfiguratie van het systeem.
- `LoopTasks` (loop): beheert de herhalende taken.
- `RouteJson` (webserver): genereert JSON data voor de webserver.
- `CacheManager`: beheert een cache voor de JSON data.

## Dependencies
- Arduino framework (Arduino.h)
- Eigen modules zoals `init`, `loop`, `webserver` in de `src` map.

## Gebruik
Compileer en upload deze code naar een Arduino compatibel bord. Zorg dat alle dependencies en modules in de juiste mappen staan.

---

