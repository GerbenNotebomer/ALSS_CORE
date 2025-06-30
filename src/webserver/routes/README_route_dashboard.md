# RouteDashboard Module

Deze module verzorgt de `/dashboard` route van de embedded webserver (ESP32/ESP8266) en genereert een dynamische dashboardpagina met realtime data van INA3221 sensoren en batterijstatus.

---

## Functionaliteit

- Genereert een volledige HTML-dashboardpagina met kanaal- en batterijinformatie.  
- Integreert meertalige labels via `langManager`.  
- Stijlt het dashboard met inline CSS voor overzichtelijke layout.  
- Voegt JavaScript toe voor periodiek ophalen en bijwerken van sensordata via een REST endpoint (`/data.json`).  
- Ondersteunt dynamische selectie van batterijen via dropdown.  
- Registreert de `/dashboard` route in de AsyncWebServer en koppelt deze aan de handler.

---

## Gebruik

### Registreren van de route

```cpp
#include "route_dashboard.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup() {
    RouteDashboard::registerRoutes(server);
    server.begin();
}

Openen van het dashboard
Navigeer in een browser naar http://<ip-van-je-esp>/dashboard om de pagina te bekijken.

Belangrijkste functies

String generateDashboardPage();
// Genereert de HTML pagina als string.

void handleDashboard(AsyncWebServerRequest *request);
// Handler voor HTTP GET op /dashboard.

void registerRoutes(AsyncWebServer &server);
// Registreert de /dashboard route bij de webserver.
Afhankelijkheden
Arduino.h

ESPAsyncWebServer.h

langManager voor vertalingen (extern in project)

HtmlUtils voor pagina rendering (extern in project)

Voorbeeld
cpp
Kopiëren
Bewerken
void RouteDashboard::handleDashboard(AsyncWebServerRequest *request) {
    String html = generateDashboardPage();
    request->send(200, "text/html", html);
}
Auteur
G.K. Notebomer – ALSS Webserver
2025, MIT License