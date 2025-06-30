# RouteJson Module

Deze module verzorgt het samenstellen en serveren van realtime JSON-data voor de embedded webserver (ESP32/ESP8266). De data omvat stroom- en spanningsmetingen, sensorwaarden en batterijstatussen.

---

## Functionaliteit

- Samenstellen van een JSON-object met actuele data van INA3221 stroom/voltage kanalen.  
- Meting en rapportage van stroom (A), spanning (V), vermogen (W), totaal Ah en Wh per kanaal.  
- Sensordata van AHT20 en BMP280 sensoren: temperatuur, luchtvochtigheid en luchtdruk.  
- Berekening van batterijstatus, laadtoestand (laden, ontladen, rust) en resterende tijd op basis van stroomverbruik.  
- Endpoint `/data.json` dat deze JSON data serveert via een HTTP GET verzoek.  
- Cache-mechanisme om de data efficiënt te serveren en bij te werken op verzoek (`refresh` parameter).

---

## Gebruik

### JSON data opbouwen

```cpp
StaticJsonDocument<2048> jsonDoc;
RouteJson::buildDataJSON(jsonDoc);

Route registreren in webserver

AsyncWebServer server(80);

void setup() {
    RouteJson::registerRoutes(server);
    server.begin();
}
JSON structuur

{
  "channels": [
    {
      "name": "MPPT",
      "current": 1.23,
      "power": 12.34,
      "voltage": 24.56,
      "totalAh": 123.45,
      "totalWh": 456.78,
      "batteryCapacityAh": 100.0
    },
    {
      "name": "Accu 1",
      "current": -0.45,
      "power": -10.11,
      "voltage": 12.34,
      "totalAh": 50.0,
      "totalWh": 200.0,
      "batteryCapacityAh": 80.0
    },
    ...
  ],
  "datetime": "2025-06-30 14:45:00",
  "dag": "Dinsdag",
  "batteries": [
    {
      "state": 0,
      "stateText": "Laden",
      "stateOfCharge": 75.5,
      "days": 1,
      "hours": 3,
      "minutes": 15
    },
    {
      "state": 2,
      "stateText": "Ontladen",
      "stateOfCharge": 60.0,
      "days": -1,
      "hours": -1,
      "minutes": -1
    }
  ]
}
Afhankelijkheden
ArduinoJson voor JSON verwerking.

ESPAsyncWebServer voor webserver functionaliteit.

INA3221_Module en AHT20BMP280_Module voor sensor- en meetdata (singleton instances).

CacheManager voor efficiënte data caching en vernieuwing.

Licentie
MIT License © 2025 G.K. Notebomer

Auteur
G.K. Notebomer – ALSS Webserver