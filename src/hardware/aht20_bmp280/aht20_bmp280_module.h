#pragma once
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Arduino.h>
#include "sensor_data.h"

#define PRESSURE_HISTORY_SIZE 10

class AHT20BMP280_Module
{
public:
    static AHT20BMP280_Module &getInstance();

    void begin();
    const SensorData &readSensors();
    String calculateForecast();
    int getMeasurementCount() const;
    int getRequiredMeasurementCount() const;

private:
    AHT20BMP280_Module() = default;
    AHT20BMP280_Module(const AHT20BMP280_Module &) = delete;
    AHT20BMP280_Module &operator=(const AHT20BMP280_Module &) = delete;

    void updatePressureHistory(float pressure);
    // String calculateForecast();

    Adafruit_AHTX0 aht;
    Adafruit_BMP280 bmp;

    SensorData internalData{};
    float pressureHistory[PRESSURE_HISTORY_SIZE] = {0};
    unsigned long lastForecastTime = 0;
};
