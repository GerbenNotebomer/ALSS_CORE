#include "route_json.h"
#include <ArduinoJson.h>
#include "../../common/common_includes.h"
#include "../cache_manager.h"

//  --  buildDataJSON  --  //
void RouteJson::buildDataJSON(StaticJsonDocument<2048> &jsonDoc)
{
    INA3221_Module &ina3221 = INA3221_Module::getInstance();

    JsonArray channelsArray = jsonDoc.createNestedArray("channels");

    // ==== Stroommetingen ====
    float currentCh1 = ina3221.channels[KANAAL_MPPT].filteredCurrent_A;
    float currentCh2 = ina3221.channels[KANAAL_ACCU_1].filteredCurrent_A;
    float currentCh3 = ina3221.channels[KANAAL_ACCU_2].filteredCurrent_A;

    float laadStroom = (currentCh1 < 0) ? -currentCh1 : 0;
    float verbruik = 0;
    if (currentCh2 > 0)
        verbruik += currentCh2;
    if (currentCh3 > 0)
        verbruik += currentCh3;
    float nettoVerbruikA = verbruik - laadStroom;

    // ==== Kanaaldata ====
    for (int i = 1; i <= NUM_INA_CHANNELS; i++)
    {
        JsonObject chObj = channelsArray.createNestedObject();
        chObj["name"] = ina3221.channelSettings[i].name;
        chObj["current"] = ina3221.channels[i].filteredCurrent_A;
        chObj["power"] = ina3221.channels[i].filteredPower_W;
        chObj["voltage"] = ina3221.channels[i].filteredBusVoltage;
        chObj["totalAh"] = ina3221.channels[i].totalAh;
        chObj["totalWh"] = ina3221.channels[i].totalWh;
        chObj["batteryCapacityAh"] = ina3221.channels[i].batteryCapacityAh;
    }

    // ==== Sensoren ====
    AHT20BMP280_Module &sensor = AHT20BMP280_Module::getInstance(); // Singleton correct
    const SensorData &sensorData = sensor.readSensors();

    JsonObject sensorObj = channelsArray.createNestedObject();
    sensorObj["name"] = "AHT20/BMP280";
    sensorObj["temperatureAHT"] = sensorData.ahtTemperature;
    sensorObj["humidityAHT"] = sensorData.ahtHumidity;
    sensorObj["temperatureBMP"] = sensorData.bmpTemperature;
    sensorObj["pressure"] = sensorData.pressure;

    // ==== Tijd ====
    jsonDoc["datetime"] = rtcManager.getFormattedDateTime();
    jsonDoc["dag"] = rtcManager.getDag();

    // ==== Accu-informatie ====
    std::vector<int> geselecteerdeAccuKanalen = {KANAAL_ACCU_1, KANAAL_ACCU_2};
    JsonArray batteries = jsonDoc.createNestedArray("batteries");

    const float drempel = 0.05;

    for (size_t i = 0; i < geselecteerdeAccuKanalen.size(); ++i)
    {
        int kanaal = geselecteerdeAccuKanalen[i];
        const auto &accu = ina3221.channels[kanaal];

        JsonObject batt = batteries.createNestedObject();

        if (nettoVerbruikA < -drempel)
        {
            batt["state"] = 0;
            batt["stateText"] = "Laden";
        }
        else if (nettoVerbruikA > drempel)
        {
            batt["state"] = 2;
            batt["stateText"] = "Ontladen";
        }
        else
        {
            batt["state"] = 3;
            batt["stateText"] = "Rust";
        }

        batt["stateOfCharge"] = accu.stateOfCharge;

        if (i == 0)
        {
            float capaciteitAh = accu.batteryCapacityAh;
            if (capaciteitAh > 0 && abs(nettoVerbruikA) > drempel)
            {
                float uren = capaciteitAh / abs(nettoVerbruikA);
                int dagen = uren / 24;
                int urenRest = ((int)uren) % 24;
                int minuten = (uren - (int)uren) * 60;

                batt["days"] = dagen;
                batt["hours"] = urenRest;
                batt["minutes"] = minuten;
            }
            else
            {
                batt["days"] = -1;
                batt["hours"] = -1;
                batt["minutes"] = -1;
            }
        }
        else
        {
            batt["days"] = -1;
            batt["hours"] = -1;
            batt["minutes"] = -1;
        }
    }
}

//  --  handleDataJSON  --  //
void handleDataJSON(AsyncWebServerRequest *request)
{
    if (request->hasParam("refresh"))
    {
        dataJsonCache->forceUpdate();
    }

    request->send(200, "application/json", dataJsonCache->getCachedString());
}

//  --  registerRoutes  --  //
void RouteJson::registerRoutes(AsyncWebServer &server)
{
    server.on("/data.json", HTTP_GET, handleDataJSON);
    server.on("/data/language", HTTP_GET, handleLanguageJSON);  // <-- nieuwe regel
}

/*
void RouteJson::registerRoutes(AsyncWebServer &server)
{
    server.on("/data.json", HTTP_GET, handleDataJSON);
}
*/


//  --  handleLanguageJSON  --  //
void handleLanguageJSON(AsyncWebServerRequest *request)
{
    File file = LittleFS.open("/lang/translations_app.json", "r");

    if (!file || file.isDirectory())
    {
        request->send(404, "application/json", "{\"error\":\"File not found\"}");
        return;
    }

    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/lang/translations_app.json", "application/json");
    request->send(response);
}
