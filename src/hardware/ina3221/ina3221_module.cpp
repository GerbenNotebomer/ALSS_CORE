#include "ina3221_module.h"

SDL_Arduino_INA3221 ina3221;
ChannelSettings channelSettings[4];
ChannelData channels[4];

// --- ChannelSettings_EEPROM struct ---
struct ChannelSettings_EEPROM
{
    float shuntResistor;
    float mvOffset;
    float mvGain;
    bool invertCurrent;
    float currentCorrection;
    float voltageCorrection;
    float shuntVoltage_mV;
    float shuntCurrent_A;
};

// --- ChannelSettings methods ---
void ChannelSettings::saveToEEPROM(int start) const
{
    ChannelSettings_EEPROM tmp;
    tmp.shuntResistor = shuntResistor;
    tmp.mvOffset = mvOffset;
    tmp.mvGain = mvGain;
    tmp.invertCurrent = invertCurrent;
    tmp.currentCorrection = currentCorrection;
    tmp.voltageCorrection = voltageCorrection;
    tmp.shuntVoltage_mV = shuntVoltage_mV;
    tmp.shuntCurrent_A = shuntCurrent_A;

    EEPROM.put(start, tmp);
}

//  --  loadFromEEPROM  --  //
void ChannelSettings::loadFromEEPROM(int start)
{
    ChannelSettings_EEPROM tmp;
    EEPROM.get(start, tmp);

    shuntResistor = tmp.shuntResistor;
    mvOffset = tmp.mvOffset;
    mvGain = tmp.mvGain;
    invertCurrent = tmp.invertCurrent;
    currentCorrection = tmp.currentCorrection;
    voltageCorrection = tmp.voltageCorrection;
    shuntVoltage_mV = tmp.shuntVoltage_mV;
    shuntCurrent_A = tmp.shuntCurrent_A;
}

size_t ChannelSettings::size()
{
    return sizeof(ChannelSettings_EEPROM);
}

//// ---- ChannelData methods ----  ////
// --  reset  --  //
void ChannelData::reset(float capacity)
{
    filteredCurrent_A = 0;
    filteredPower_W = 0;
    filteredBusVoltage = 0;
    totalAh = 0;
    totalWh = 0;
    batteryCapacityAh = capacity;
    // lastEnergyCalcDateTime = DateTime(0);
    lastEnergyCalcDateTime = DateTime((uint32_t)0);

    stateOfCharge = 0.0f;
    state = BATTERY_DISCHARGING;
}

//// ---- INA3221_Module methods ----  /////
//  --  begin  --  //
void INA3221_Module::begin()
{
    ina3221.begin();
    setDefaultCalibration();

    // Debug output van alle geladen waardes
    for (int i = 1; i <= NUM_INA_CHANNELS; i++)
    {
        DEBUG_LOGF("[INA3221_Module] Kanaal %d (%s) geladen instellingen:", i, channelSettings[i].name.c_str());
        DEBUG_LOGF("  shuntResistor: %.6f", channelSettings[i].shuntResistor);
        DEBUG_LOGF("  mvOffset: %.6f", channelSettings[i].mvOffset);
        DEBUG_LOGF("  mvGain: %.6f", channelSettings[i].mvGain);
        DEBUG_LOGF("  invertCurrent: %s", channelSettings[i].invertCurrent ? "true" : "false");
        DEBUG_LOGF("  currentCorrection: %.6f", channelSettings[i].currentCorrection);
        DEBUG_LOGF("  voltageCorrection: %.6f", channelSettings[i].voltageCorrection);
        DEBUG_LOGF("  shuntVoltage_mV: %.6f", channelSettings[i].shuntVoltage_mV);
        DEBUG_LOGF("  shuntCurrent_A: %.6f", channelSettings[i].shuntCurrent_A);

        DEBUG_LOGF("  totalAh: %.6f", channels[i].totalAh);
        DEBUG_LOGF("  batteryCapacityAh: %.6f", channels[i].batteryCapacityAh);
    }
}

//  --  setDefaultCalibration  --  //
void INA3221_Module::setDefaultCalibration()
{
    channelSettings[KANAAL_MPPT] = {"MPPT", 0.0015f, 0.2f, 1.0f, true, 1.0f, 1.0f, 75.0f, 50.0f};
    channelSettings[KANAAL_ACCU_1] = {"Accu 1", 0.0015f, 0.125f, 1.0f, false, 1.0f, 1.0f, 75.0f, 50.0f};
    channelSettings[KANAAL_ACCU_2] = {"Accu 2", 0.0015f, 0.0f, 1.0f, false, 1.0f, 1.0f, 75.0f, 50.0f};

    channels[KANAAL_ACCU_1].reset(200.0f);
    channels[KANAAL_ACCU_2].reset(72.0f);
}

//  --  updateAllChannels  --  //
void INA3221_Module::updateAllChannels()
{
    for (int channel = 1; channel <= NUM_INA_CHANNELS; channel++)
    {
        updateChannelData(channel);
    }
}

//  --  updateChannelData  --  //
void INA3221_Module::updateChannelData(int channel)
{
    DateTime now = rtcManager.now();

    ChannelData &ch = channels[channel];
    ChannelSettings &cfg = channelSettings[channel];

    cfg.shuntResistor = cfg.shuntVoltage_mV / (cfg.shuntCurrent_A * 1000.0f);

    float rawBusVoltage = ina3221.getBusVoltage_V(channel);
    float busVoltage = rawBusVoltage * cfg.voltageCorrection;

    float shuntVoltage_mV = ina3221.getShuntVoltage_mV(channel);
    float current_A = (shuntVoltage_mV - cfg.mvOffset) / (cfg.shuntResistor * 1000.0f) * cfg.mvGain * cfg.currentCorrection;

    if (cfg.invertCurrent)
    {
        current_A *= -1;
    }

    float alpha = (channel == KANAAL_MPPT) ? 0.05f : 0.1f;

    ch.filteredCurrent_A = alpha * current_A + (1 - alpha) * ch.filteredCurrent_A;
    current_A = ch.filteredCurrent_A;

    float loadVoltage = busVoltage - (current_A * cfg.shuntResistor);
    float power_W = loadVoltage * current_A;

    ch.filteredPower_W = alpha * power_W + (1 - alpha) * ch.filteredPower_W;
    ch.filteredBusVoltage = alpha * busVoltage + (1 - alpha) * ch.filteredBusVoltage;

    if (now.year() == ch.lastEnergyCalcDateTime.year() &&
        now.month() == ch.lastEnergyCalcDateTime.month() &&
        now.day() == ch.lastEnergyCalcDateTime.day())
    {
        float deltaHours = (now.unixtime() - ch.lastEnergyCalcDateTime.unixtime()) / 3600.0f;
        ch.totalAh += current_A * deltaHours;
        ch.totalWh += power_W * deltaHours;
    }
    else
    {
        ch.totalAh = 0.0f;
        ch.totalWh = 0.0f;
    }

    ch.lastEnergyCalcDateTime = now;

    float soc = estimateSOC_CoulombCounting(ch.totalAh, ch.batteryCapacityAh);
    if (soc < 5.0f || soc > 100.0f)
    {
        soc = estimateSOC_VoltageOnly(busVoltage);
    }

    ch.stateOfCharge = soc;

    BatteryState state;
    if (current_A > 0.5f && soc < 95.0f)
    {
        state = BATTERY_CHARGING;
    }
    else if (current_A <= 0.5f && soc >= 95.0f)
    {
        state = BATTERY_FULL;
    }
    else
    {
        state = BATTERY_DISCHARGING;
    }
    ch.state = state;

    float estimatedHours = estimateRemainingHours(soc, ch.batteryCapacityAh, fabs(current_A));

    DEBUG_LOGF("[INA3221_Channel%d] Voltage=%.3fV, Current=%.3fA, Power=%.3fW, SOC=%.1f%%, Est hours=%.1f",
               channel, busVoltage, current_A, power_W, soc, estimatedHours);
}

//  --  estimateSOC_VoltageOnly  --  //
float INA3221_Module::estimateSOC_VoltageOnly(float voltage)
{
    if (voltage >= 12.85f)
        return 100.0f;
    if (voltage <= 11.6f)
        return 0.0f;

    return ((voltage - 11.6f) / (12.85f - 11.6f)) * 100.0f;
}

// --  estimateSOC_CoulombCounting  -- //
float INA3221_Module::estimateSOC_CoulombCounting(float totalAh, float capacityAh)
{
    if (capacityAh <= 0)
        return 0;
    return (totalAh / capacityAh) * 100.0f;
}

//  --  estimateRemainingHours  --  //
float INA3221_Module::estimateRemainingHours(float soc, float capacityAh, float currentA)
{
    if (currentA < 0.05f)
        return 999.0f;
    if (soc <= 0)
        return 0.0f;
    float remainingAh = capacityAh * (soc / 100.0f);
    return remainingAh / currentA;
}

//  --  calculateRemainingTime  --  //
RemainingTime INA3221_Module::calculateRemainingTime(float batteryCapacityAH, float currentCh1, float currentCh2, float currentCh3)
{
    float totalCurrent = fabs(currentCh1) + fabs(currentCh2) + fabs(currentCh3);
    if (totalCurrent < 0.05f)
    {
        return RemainingTime{99, 99, 99};
    }

    float remainingHours = batteryCapacityAH / totalCurrent;

    int days = static_cast<int>(remainingHours / 24);
    remainingHours -= days * 24;
    int hours = static_cast<int>(remainingHours);
    remainingHours -= hours;
    int minutes = static_cast<int>(remainingHours * 60);

    return RemainingTime{days, hours, minutes};
}
