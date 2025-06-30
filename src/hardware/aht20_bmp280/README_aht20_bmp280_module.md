# AHT20BMP280_Module

## Overzicht
Deze module verzorgt de integratie van twee sensoren:

- **AHT20** voor temperatuur en relatieve luchtvochtigheid.
- **BMP280** voor temperatuur en luchtdrukmetingen.

De module leest deze sensorwaarden uit, berekent de atmosferische druk op zeeniveau en houdt een historische drukreeks bij om op basis daarvan een weersvoorspelling te maken.

---

## Structuren en Variabelen

### SensorData
Structuur waarin actuele meetwaarden worden opgeslagen:
- `ahtTemperature` (°C)
- `ahtHumidity` (% RV)
- `bmpTemperature` (°C)
- `rawPressure` (hPa)
- `pressure` (hPa) — atmosferische druk gecorrigeerd naar zeeniveau
- `forecast` (string) — voorspelling gebaseerd op druktrend

### pressureHistory
Array met de laatste `PRESSURE_HISTORY_SIZE` (10) drukmetingen, gebruikt voor trendanalyse.

### lastForecastTime
Timestamp van de laatste update van de weersvoorspelling, om deze periodiek te updaten.

---

## Belangrijkste methoden

- `begin()`
  - Initialiseert de AHT20 en BMP280 sensoren.
  - Stelt BMP280 in op bepaalde meetparameters (sampling, filter, standby).
  - Logt succes/foutmeldingen via debug output.

- `readSensors()`
  - Leest temperatuur en luchtvochtigheid uit AHT20.
  - Leest temperatuur en ruwe druk uit BMP280.
  - Berekent druk op zeeniveau met formule rekening houdend met een vaste hoogte (2 meter).
  - Update drukhistorie en berekent eventueel een nieuwe weersvoorspelling.
  - Retourneert een referentie naar de struct met actuele sensorwaarden.

- `updatePressureHistory(float pressure)`
  - Verplaatst oude drukmetingen een stap naar achter in de array.
  - Plaatst nieuwste drukwaarde vooraan.
  - Gebruikt voor trendanalyse.

- `calculateForecast()`
  - Analyseert de druktrend over de historische data.
  - Bepaalt het weertype (zoals zonnig, stabiel, stormachtig) op basis van veranderingen in luchtdruk.
  - Gebruikt drempelwaarden en absolute drukwaarden om voorspelling te classificeren.
  - Retourneert een string met de voorspelling, opgehaald via een taalmanager (lokalisatie).

- `getMeasurementCount()`
  - Geeft het aantal beschikbare drukmetingen terug (aantal niet-nul entries in `pressureHistory`).

- `getRequiredMeasurementCount()`
  - Retourneert constante `PRESSURE_HISTORY_SIZE`, het aantal metingen benodigd voor betrouwbare voorspelling.

---

## Voorbeeldgebruik

```cpp
#include "aht20_bmp280_module.h"

void setup() {
    Serial.begin(115200);

    AHT20BMP280_Module &sensorModule = AHT20BMP280_Module::getInstance();
    sensorModule.begin();
}

void loop() {
    AHT20BMP280_Module &sensorModule = AHT20BMP280_Module::getInstance();

    const SensorData &data = sensorModule.readSensors();

    Serial.printf("AHT20 Temp: %.2f °C, Humidity: %.2f %%\n", data.ahtTemperature, data.ahtHumidity);
    Serial.printf("BMP280 Temp: %.2f °C, Pressure (sea level): %.2f hPa\n", data.bmpTemperature, data.pressure);
    Serial.printf("Forecast: %s\n", data.forecast.c_str());

    delay(2000);
}

## Tips & Toelichting

- **Hoogtecorrectie:**  
  De druk op zeeniveau wordt berekend met een vaste hoogte van 2 meter. Pas deze waarde aan naar jouw locatie voor nauwkeurigere resultaten.

- **Weersvoorspelling:**  
  De voorspelling baseert zich op de verandering van luchtdruk over tijd. Het algoritme is simpel en bedoeld als ruwe indicatie.

- **Lokalisatie:**  
  Teksten voor voorspellingen komen uit een taalmanager (`langManager.get()`), waardoor meertalige ondersteuning mogelijk is.

- **Interval updates:**  
  De weersvoorspelling wordt niet bij elke meting geüpdatet, maar periodiek volgens `Timers::Timers_forecastInterval`.

- **Debug output:**  
  Via `DEBUG_LOG` en `DEBUG_LOGF` worden details en statusberichten gelogd, wat handig is voor ontwikkeling en troubleshooting.