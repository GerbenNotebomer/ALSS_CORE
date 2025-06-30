# ALSS Webserver Dashboard

Een embedded webserver-applicatie voor ESP32/ESP8266 die realtime sensor- en batterijdata weergeeft via een overzichtelijk en meertalig dashboard.

---

## Overzicht

Dit project draait op een ESP32 of ESP8266 en serveert een webinterface waarop je live data kunt bekijken van INA3221 stroom/voltage kanalen en batterijstatussen. De webserver gebruikt `ESPAsyncWebServer` en toont dynamisch bijgewerkte informatie via REST API endpoints en JavaScript.

De dashboardpagina is volledig meertalig en biedt een moderne, responsieve gebruikerservaring met realtime updates, statusindicatoren en visuele batterij-‘charge bars’.

---

## Functionaliteit

- **Realtime dashboard**: toont spanning, stroom, vermogen van kanalen en batterijstatussen.  
- **Meertalige interface**: alle labels en teksten worden geladen via een vertaalmanager (`langManager`).  
- **Dynamische data-updates**: met JavaScript fetch API om elke seconde nieuwe data op te halen zonder herladen.  
- **Batterijselectie**: kies welke batterijdata getoond wordt via een dropdown.  
- **Gestileerde UI**: duidelijke lay-out met CSS en visuele progress bars voor batterijpercentage.  
- **Gebruiksvriendelijke structuur**: gescheiden routes en rendering functies voor overzichtelijkheid.  
- **JSON REST API endpoint `/data.json`**: levert alle actuele sensor- en batterijdata in JSON-formaat voor de frontend.  
- **Debug log webinterface**: live debuglogs bekijken, debugmodules dynamisch aan/uit zetten, en logbuffer wissen via een gebruiksvriendelijke pagina met REST API ondersteuning.  
- **Opslagbeheer SDCard/LittleFS**: beheert SD-kaart en LittleFS voor bestandsopslag, inclusief lezen, schrijven, verwijderen en directory listings.  
- **Energielogging naar CSV**: periodiek loggen van energiewaarden naar bestanden op SD-kaart of LittleFS met bufferbeheer voor efficiënt wegschrijven.  
- **Configuratieparameters beheer**: laadt, bewaart en beheert projectparameters vanuit een bestand op SD-kaart met fallback naar standaardwaarden.  
- **Dynamische parameterupdates**: ondersteunt reflectie-achtige functies voor uitlezen en bijwerken van parameters via string key=value paren.  
- **Parameters voor WiFi, timers en taal**: centrale plek voor belangrijke instellingen met eenvoudige API voor aanpassing.

---

## Technische details

- Platform: ESP32 / ESP8266  
- Webserver: ESPAsyncWebServer  
- Taal: C++ met Arduino framework  
- Belangrijke bestanden/modules:  
  - `route_dashboard.cpp` / `.h` — dashboard pagina en routes  
  - `route_json.cpp` / `.h` — JSON data endpoint `/data.json`  
  - `route_debug_log.cpp` / `.h` — debug log webinterface en API  
  - `HtmlUtils` — hulpfuncties voor HTML-rendering  
  - `langManager` — vertaalmanager voor meertaligheid  
  - `CacheManager` — cache systeem voor efficiënte JSON data updates  
- Data wordt opgehaald via het endpoint `/data.json` dat realtime metingen en batterijstatussen retourneert.  
- Debug logs worden beheerd en weergegeven via een aparte webpagina met live updates en instellingen.  
- Gebruik van het Singleton-patroon in modules zoals `AHT20BMP280_Module` om te garanderen dat er slechts één instantie is die de sensorinterface beheert.  
- Moderne C++ technieken (zoals `= delete` voor copy-constructor en assignment-operator) worden gebruikt om Singleton-eigenschappen af te dwingen en de codebase overzichtelijk en veilig te houden.  

---

## Installatie en gebruik

1. Clone deze repository  
2. Open in Arduino IDE of PlatformIO  
3. Zorg dat dependencies (ESPAsyncWebServer, ArduinoJson, HtmlUtils, langManager) beschikbaar zijn  
4. Compileer en upload naar je ESP32/ESP8266  
5. Verbind met het WiFi netwerk van het apparaat of stel WiFi in in de code  
6. Navigeer in je browser naar `http://<ip-van-je-esp>/dashboard` om het dashboard te openen  
7. Voor debug logs navigeer naar `http://<ip-van-je-esp>/debuglog`
8. **Upload de `/data` map met bestanden (zoals `translations.json`, CSS, logo’s) naar LittleFS** via de LittleFS uploader van de Arduino IDE of PlatformIO, zodat de webserver deze kan gebruiken.  

---

## Benodigde kennis en vaardigheden

- **Basiskennis C++ en Arduino framework**: je moet comfortabel zijn met programmeren in C++ en bekend zijn met de Arduino-omgeving.  
- **Ervaring met ESP32/ESP8266**: basiskennis van deze microcontrollers, zoals het instellen van WiFi en het uploaden van code, is vereist.  
- **Webtechnologieën (basis)**: begrip van REST API’s, JSON en frontend technologieën zoals JavaScript en HTML helpt bij het begrijpen van het dashboard en het uitbreiden ervan.  
- **Debugging en logging**: kennis van debugmethodes en het interpreteren van logbestanden is handig om problemen op te sporen en de debugfunctionaliteit effectief te gebruiken.  
- **Begrip van softwareontwerppatronen**: zoals het Singleton-patroon dat wordt gebruikt voor hardwaremodules, om te kunnen navigeren in de architectuur en uitbreidingen veilig toe te passen.

---

## Voorbeeldcode voor registratie routes

```cpp
#include <ESPAsyncWebServer.h>
#include "route_dashboard.h"
#include "route_json.h"
#include "route_debug_log.h"

AsyncWebServer server(80);

void setup() {
    RouteDashboard::registerRoutes(server);
    RouteJson::registerRoutes(server);
    RouteDebugLog::registerRoutes(server);
    server.begin();
}

JSON data endpoint /data.json
Dit endpoint levert actuele data in JSON-formaat, waaronder:

INA3221 kanaalmetingen (stroom, spanning, vermogen, totale Ah/Wh)

Sensorwaarden van AHT20 en BMP280 (temperatuur, luchtvochtigheid, druk)

Batterijstatussen met laad-/ontlaadindicatie en resterende tijd

Datum- en tijdinformatie

De JSON data wordt ge-cachet en kan geforceerd ververst worden met een refresh parameter.

Licentie
MIT License © 2025 G.K. Notebomer

Auteur
G.K. Notebomer – ALSS Webserver