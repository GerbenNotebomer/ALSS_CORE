#include <math.h>
#include "aht20_bmp280_module.h"
#include "../../common/common_includes.h"

//  --  AHT20BMP280_Module &AHT20BMP280_Module::getInstance()  --  //
AHT20BMP280_Module &AHT20BMP280_Module::getInstance()
{
    static AHT20BMP280_Module instance;
    return instance;
}

//  --  begin  --  //
void AHT20BMP280_Module::begin()
{
    if (!aht.begin())
    {
        DEBUG_LOGF("[begin()] ❌ AHT20 niet gevonden!");
    }
    else
    {
        DEBUG_LOGF("[begin()] ✅ AHT20 OK");
    }

    if (!bmp.begin(0x77))
    {
        DEBUG_LOGF("[begin()] ❌ BMP280 niet gevonden!");
    }
    else
    {
        DEBUG_LOGF("[begin()] ✅ BMP280 OK");
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                        Adafruit_BMP280::SAMPLING_X2,
                        Adafruit_BMP280::SAMPLING_X16,
                        Adafruit_BMP280::FILTER_X16,
                        Adafruit_BMP280::STANDBY_MS_500);
    }
}

//  --  SensorData &AHT20BMP280_Module::readSensors  --  //
const SensorData &AHT20BMP280_Module::readSensors()
{
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    internalData.ahtTemperature = temp.temperature;
    internalData.ahtHumidity = humidity.relative_humidity;

    internalData.bmpTemperature = bmp.readTemperature();
    float rawPressure = bmp.readPressure() / 100.0f; // hPa
    internalData.rawPressure = rawPressure;

    const float altitude = 2.0f; // meter boven zeeniveau
    float seaLevelPressure = rawPressure / pow(1.0f - (altitude / 44330.0f), 5.255f);
    internalData.pressure = seaLevelPressure;

    DEBUG_LOGF("[readSensors] AHT: Temp = %.2f°C, Humidity = %.2f%%", internalData.ahtTemperature, internalData.ahtHumidity);
    DEBUG_LOGF("[readSensors] BMP: Temp = %.2f°C, Raw Pressure = %.2f hPa, Sea Level Pressure = %.2f hPa",
               internalData.bmpTemperature, rawPressure, seaLevelPressure);

    unsigned long currentMillis = millis();
    if (currentMillis - lastForecastTime >= Timers::Timers_forecastInterval)
    {
        updatePressureHistory(seaLevelPressure);
        internalData.forecast = calculateForecast();
        DEBUG_LOGF("[readSensors] Forecast updated: %s", internalData.forecast.c_str());
        lastForecastTime = currentMillis;
    }

    return internalData;
}

//  --  updatePressureHistory  --  //
void AHT20BMP280_Module::updatePressureHistory(float pressure)
{
    DEBUG_LOG("[updatePressureHistory] Updating pressure history...");
    for (int i = PRESSURE_HISTORY_SIZE - 1; i > 0; i--)
    {
        pressureHistory[i] = pressureHistory[i - 1];
    }
    pressureHistory[0] = pressure;

    DEBUG_LOG("[updatePressureHistory] Current pressure history:");
    for (int i = 0; i < PRESSURE_HISTORY_SIZE; i++)
    {
        DEBUG_LOGF("  [%02d] = %.2f hPa", i, pressureHistory[i]);
    }
}

//  -- calculateForecast  --  //
String AHT20BMP280_Module::calculateForecast()
{
    if (pressureHistory[PRESSURE_HISTORY_SIZE - 1] == 0)
    {
        DEBUG_LOG("[calculateForecast] Niet genoeg data voor voorspelling.");
        return langManager.get("forecast.no_data");
    }

    float oldPressure = pressureHistory[PRESSURE_HISTORY_SIZE - 1];
    float recentPressure = pressureHistory[0];
    float delta = recentPressure - oldPressure;
    float absPressure = recentPressure;

    DEBUG_LOGF("[calculateForecast] Druktrend analyse: recent = %.2f hPa, oud = %.2f hPa, delta = %.2f hPa",
               recentPressure, oldPressure, delta);

    if (delta > 2.0f)
    {
        return (absPressure > 1015)
                   ? langManager.get("forecast.sun_dry")
                   : langManager.get("forecast.improving");
    }
    else if (delta > 0.5f)
    {
        return langManager.get("forecast.partly_clear");
    }
    else if (delta < -2.0f)
    {
        return (absPressure < 1000)
                   ? langManager.get("forecast.storm")
                   : langManager.get("forecast.degrading");
    }
    else if (delta < -0.5f)
    {
        return langManager.get("forecast.slight_degrading");
    }
    else
    {
        if (absPressure >= 1010 && absPressure <= 1020)
            return langManager.get("forecast.stable");
        else if (absPressure < 1005)
            return langManager.get("forecast.cloudy");
        else
            return langManager.get("forecast.uncertain");
    }
}

//  -- calculateForecast  --  //
int AHT20BMP280_Module::getMeasurementCount() const
{
    int count = 0;
    for (int i = 0; i < PRESSURE_HISTORY_SIZE; i++)
    {
        if (pressureHistory[i] > 0.0f)
        {
            count++;
        }
    }
    return count;
}

//  -- getRequiredMeasurementCount  --  //
int AHT20BMP280_Module::getRequiredMeasurementCount() const
{
    return PRESSURE_HISTORY_SIZE;
}
