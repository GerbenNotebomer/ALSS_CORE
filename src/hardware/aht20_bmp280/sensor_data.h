#pragma once
#include <Arduino.h>
#include <string>

struct SensorData
{
    float ahtTemperature = 0.0f; // Temperatuur van AHT20 sensor
    float ahtHumidity = 0.0f;    // Luchtvochtigheid van AHT20

    float bmpTemperature = 0.0f; // Temperatuur van BMP280 sensor
    float rawPressure = 0.0f;    // Gemeten druk (rauwe waarde)
    float pressure = 0.0f;       // Druk gecorrigeerd naar zeeniveau

    String forecast = "Onvoldoende data"; // Weersvoorspelling als string
};
