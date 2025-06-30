
#pragma once

#include <EEPROM.h>
#include "../rtc/rtc_pcf8563_module.h"
#include <SDL_Arduino_INA3221.h>
#include "../../common/common_includes.h"

// Constants
#define NUM_INA_CHANNELS 3

struct RemainingTime
{
    int days;
    int hours;
    int minutes;
};

enum BatteryState
{
    BATTERY_CHARGING,
    BATTERY_FULL,
    BATTERY_DISCHARGING
};

// Channel index constants (voor overzicht)
enum
{
    KANAAL_MPPT = 1,
    KANAAL_ACCU_1 = 2,
    KANAAL_ACCU_2 = 3
};

// Struct voor EEPROM opslag (private, in cpp)
struct ChannelSettings_EEPROM;

struct ChannelSettings
{
    String name;
    float shuntResistor;
    float mvOffset;
    float mvGain;
    bool invertCurrent;
    float currentCorrection;
    float voltageCorrection;
    float shuntVoltage_mV;
    float shuntCurrent_A;

    void saveToEEPROM(int start) const;
    void loadFromEEPROM(int start);
    static size_t size();
};

struct ChannelData
{
    float filteredCurrent_A = 0;
    float filteredPower_W = 0;
    float filteredBusVoltage = 0;
    float totalAh = 0;
    float totalWh = 0;
    float batteryCapacityAh = 0;
    DateTime lastEnergyCalcDateTime;
    float stateOfCharge = 0.0f;
    BatteryState state = BATTERY_DISCHARGING;

    void reset(float capacity);
};

class INA3221_Module
{
public:
    static INA3221_Module &getInstance()
    {
        static INA3221_Module instance;
        return instance;
    }

    void begin();
    void setDefaultCalibration();
    void updateAllChannels();
    void updateChannelData(int channel);

    float estimateSOC_VoltageOnly(float voltage);
    RemainingTime calculateRemainingTime(float batteryCapacityAH, float currentCh1, float currentCh2, float currentCh3);
    float estimateSOC_CoulombCounting(float totalAh, float capacityAh);
    float estimateRemainingHours(float soc, float capacityAh, float currentA);

    ChannelSettings channelSettings[4];
    ChannelData channels[4];

private:
    INA3221_Module() = default;
    INA3221_Module(const INA3221_Module &) = delete;
    INA3221_Module &operator=(const INA3221_Module &) = delete;

    SDL_Arduino_INA3221 ina3221;
};
