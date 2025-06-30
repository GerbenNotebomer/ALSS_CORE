# Overzicht
RouteDebugLog is een module voor het beheren en weergeven van debuglogs via een webinterface op een ESP-achtige microcontroller. Het biedt een gebruiksvriendelijke pagina om debugmodules in- of uit te schakelen, logs live te bekijken, en logs te wissen.

# Functionaliteiten
- Dynamisch inschakelen/uitschakelen van debugmodules via checkboxes.
- Live weergeven van de laatste debuglogs.
- Tellen en tonen van het aantal logregels.
- Logbuffer wissen via webinterface met bevestiging.
- REST API endpoints voor log ophalen, instellingen aanpassen, en wissen.
- Gebruik van ESPAsyncWebServer en ArduinoJson voor efficiënte web- en JSON-handling.

# Bestand(en)
- `route_debug_log.cpp`  
  Implementatie van de webpagina, API-endpoints en logica.

- `route_debug_log.h`  
  Declaratie van de RouteDebugLog klasse en methoden.

# Gebruik

## 1. Routes registreren
Registreer de routes in je hoofdprogramma, bijvoorbeeld in `setup()`:

```cpp
RouteDebugLog::registerRoutes(server);

2. Debug modules configureren
De pagina toont checkboxes voor elke debugmodule (moduleNames array) waarmee je per module debugoutput kunt in- of uitschakelen.

3. Logs bekijken en wissen
De webpagina toont logs live, met automatische refresh elke 3 seconden.

Je kunt logs wissen via de "Wissen" knop, met een bevestigingsmodal.

API Endpoints
Endpoint	Methode	Beschrijving
/debuglog/view	GET	Laadt de debug log webpagina.
/debuglog/json	GET	Geeft JSON met huidige logs en teller.
/debuglog/settings	POST	Slaat debugmodule instellingen op (in- of uitschakelen).
/debuglog/clear	POST	Wis de huidige logbuffer.

Vereisten
ESP32 of ESP8266 met ESPAsyncWebServer.

ArduinoJson library.

DebugUtil module die logbuffer en module instellingen beheert.

EEPROM manager (optioneel) om instellingen persistent op te slaan.

Voorbeeld van registratie

#include <ESPAsyncWebServer.h>
#include "route_debug_log.h"

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialiseer DebugUtil etc.
  
  RouteDebugLog::registerRoutes(server);
  
  server.begin();
}
Belangrijke functies
generateDebugLogPage()
Genereert de HTML/CSS/JS van de debug log pagina met instellingen en live log view.

handleDebugLogPage()
Serveert de debug log pagina.

handleDebugLogJson()
Serveert de logs als JSON voor frontend.

handleDebugSettingsPost()
Ontvangt POST data om debugmodules te activeren/deactiveren.

handleDebugClearPost()
Wissen van de logbuffer op verzoek.

Tips
Zorg dat DebugUtil::clearLogBuffer() correct geïmplementeerd is.

Pas eventueel moduleNames en MAX_DEBUG_MODULES aan naar jouw project.

Integreer vertalingen via langManager voor meertalige ondersteuning.