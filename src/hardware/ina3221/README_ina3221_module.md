# INA3221_Module

## Overzicht
Deze module verzorgt de metingen van een INA3221 driefasige stroom- en spanningssensor. Het leest en verwerkt data van drie kanalen (MPPT, Accu 1 en Accu 2) en berekent belangrijke batterijparameters zoals stroom, vermogen, State of Charge (SOC) en geschatte resterende gebruiksduur.

Kalibratiegegevens worden opgeslagen in EEPROM zodat ze persistent zijn over herstarts. Daarnaast wordt tijdinformatie gebruikt om accu-energie statistieken nauwkeurig bij te houden.

---

## Structuren en Enums

### ChannelSettings
Bevat kalibratie- en correctieparameters voor een kanaal, zoals shuntweerstand, offset, gain, en stroom-/spanningscorrectie.

### ChannelData
Houdt actuele gemeten en gefilterde waarden bij, zoals stroom, vermogen, spanning, totale Ah en Wh, batterijcapaciteit, SOC en batterijstatus.

### RemainingTime
Structuur met resterende tijd in dagen, uren en minuten, afgeleid van huidige stroom en batterijcapaciteit.

### BatteryState
Enum met de mogelijke batterijtoestanden:
- `BATTERY_CHARGING`
- `BATTERY_FULL`
- `BATTERY_DISCHARGING`

---

## Belangrijkste functies

- `begin()`
  - Initialiseer de INA3221 sensor.
  - Laadt of zet kalibratiewaarden naar standaard.
  - Output debuginformatie over instellingen.

- `setDefaultCalibration()`
  - Zet standaard kalibratie-instellingen voor de kanalen.

- `updateAllChannels()`
  - Ververst alle kanalen door actuele sensorwaarden te lezen en verwerken.

- `updateChannelData(int channel)`
  - Leest meetwaarden voor een specifiek kanaal.
  - Filtert ruwe data.
  - Berekenen van stroom, spanning, vermogen.
  - Berekenen van SOC via Coulomb telling en/of spanningsmeting.
  - Bepaalt batterijstatus.
  - Houdt totale Ah en Wh bij met tijdstempels.

- `estimateSOC_VoltageOnly(float voltage)`
  - Schat SOC op basis van batterijspanning (voor snelle inschatting).

- `estimateSOC_CoulombCounting(float totalAh, float capacityAh)`
  - Schat SOC via integratie van stroom over tijd (nauwkeuriger).

- `estimateRemainingHours(float soc, float capacityAh, float currentA)`
  - Berekent resterende batterijduur in uren.

- `calculateRemainingTime(float batteryCapacityAH, float currentCh1, float currentCh2, float currentCh3)`
  - Berekent resterende batterijduur als dagen, uren en minuten over alle drie kanalen.

---

## Voorbeeldgebruik

```cpp
#include "ina3221_module.h"

void setup() {
  Serial.begin(115200);

  // Initialiseer INA3221 module
  INA3221_Module &ina = INA3221_Module::getInstance();
  ina.begin();

  // Stel eventueel aangepaste kalibratie in of laad uit EEPROM
  // ina.channelSettings[KANAAL_MPPT].loadFromEEPROM(0);

  // Start met standaard kalibratie
  ina.setDefaultCalibration();
}

void loop() {
  INA3221_Module &ina = INA3221_Module::getInstance();

  // Update alle kanalen met nieuwe metingen
  ina.updateAllChannels();

  // Lees en print gegevens van kanaal MPPT
  ChannelData &mpptData = ina.channels[KANAAL_MPPT];
  Serial.printf("MPPT Voltage: %.2f V\n", mpptData.filteredBusVoltage);
  Serial.printf("MPPT Current: %.2f A\n", mpptData.filteredCurrent_A);
  Serial.printf("MPPT Power: %.2f W\n", mpptData.filteredPower_W);
  Serial.printf("MPPT SOC: %.1f %%\n", mpptData.stateOfCharge);

  delay(1000);
}
