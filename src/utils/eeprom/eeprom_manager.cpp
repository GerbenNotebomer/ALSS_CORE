#include "eeprom_manager.h"
#include "../debug/debug_util.h"
#include "../../hardware/ina3221/ina3221_module.h"

constexpr int EEPROM_Manager::EEPROM_VERSION;
EEPROM_Manager::DebugSettings EEPROM_Manager::debugSettings;

void EEPROM_Manager::updateDebugSettingsFromDebugUtil()
{
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        debugSettings.moduleEnabled[i] = DebugUtil::isModuleEnabled(moduleNames[i]);
    }
}

void EEPROM_Manager::saveDebugSettingsToEEPROM()
{
    updateDebugSettingsFromDebugUtil();
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(ADDR_DEBUG_SETTINGS, debugSettings);
    EEPROM.commit();
    EEPROM.end();

    DEBUG_LOGF("[saveDebugSettingsToEEPROM] Debug-instellingen opgeslagen op adres %d", ADDR_DEBUG_SETTINGS);
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        DEBUG_LOGF("[saveDebugSettingsToEEPROM]  %s: %s", moduleNames[i], debugSettings.moduleEnabled[i] ? "aan" : "uit");
    }
}

void EEPROM_Manager::loadDebugSettingsFromEEPROM()
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(ADDR_DEBUG_SETTINGS, debugSettings);
    EEPROM.end();

    DEBUG_LOGF("[loadDebugSettingsFromEEPROM] Debug-instellingen geladen van adres %d", ADDR_DEBUG_SETTINGS);
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        DebugUtil::setModuleEnabled(moduleNames[i], debugSettings.moduleEnabled[i]);
        DEBUG_LOGF("[loadDebugSettingsFromEEPROM]  %s: %s", moduleNames[i], debugSettings.moduleEnabled[i] ? "aan" : "uit");
    }
}

void EEPROM_Manager::saveCalibrationToEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int version = 0;
    EEPROM.get(ADDR_VERSION, version);
    if (version != EEPROM_VERSION)
    {
        EEPROM.put(ADDR_VERSION, EEPROM_VERSION);
        DEBUG_LOGF("[saveCalibrationToEEPROM] EEPROM versie bijgewerkt naar %d op adres %d", EEPROM_VERSION, ADDR_VERSION);
    }

    int addr = ADDR_CALIBRATION;
    for (int i = 1; i <= NUM_INA_CHANNELS; i++)
    {
        ina.channelSettings[i].saveToEEPROM(addr);
        DEBUG_LOGF("[saveCalibrationToEEPROM] Kalibratie kanaal %d opgeslagen op adres %d", i, addr);
        addr += ChannelSettings::size();
    }

    EEPROM.commit();
    EEPROM.end();
}

void EEPROM_Manager::loadCalibrationFromEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int version;
    EEPROM.get(ADDR_VERSION, version);

    if (version == EEPROM_VERSION)
    {
        DEBUG_LOGF("[loadCalibrationFromEEPROM] EEPROM versie OK (%d)", version);
        int addr = ADDR_CALIBRATION;
        for (int i = 1; i <= NUM_INA_CHANNELS; i++)
        {
            ina.channelSettings[i].loadFromEEPROM(addr);
            DEBUG_LOGF("[loadCalibrationFromEEPROM] Kalibratie kanaal %d geladen van adres %d", i, addr);
            addr += ChannelSettings::size();
        }
    }
    else
    {
        DEBUG_LOGF("[loadCalibrationFromEEPROM] Ongeldige EEPROM versie (%d), standaardinstellingen laden...", version);
        ina.setDefaultCalibration(); // <-- Correcte aanroep
    }

    EEPROM.end();
}

void EEPROM_Manager::saveAhToEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int addr = ADDR_TOTAL_AH;
    EEPROM.put(addr, ina.channels[KANAAL_ACCU_1].totalAh);
    DEBUG_LOGF("[saveAhToEEPROM] Accu 1 Ah (%f) opgeslagen op adres %d", ina.channels[KANAAL_ACCU_1].totalAh, addr);
    addr += sizeof(float);
    EEPROM.put(addr, ina.channels[KANAAL_ACCU_2].totalAh);
    DEBUG_LOGF("[saveAhToEEPROM] Accu 2 Ah (%f) opgeslagen op adres %d", ina.channels[KANAAL_ACCU_2].totalAh, addr);
    EEPROM.commit();
    EEPROM.end();
}

void EEPROM_Manager::loadAhFromEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int addr = ADDR_TOTAL_AH;
    EEPROM.get(addr, ina.channels[KANAAL_ACCU_1].totalAh);
    DEBUG_LOGF("[loadAhFromEEPROM] Accu 1 Ah (%f) geladen van adres %d", ina.channels[KANAAL_ACCU_1].totalAh, addr);
    addr += sizeof(float);
    EEPROM.get(addr, ina.channels[KANAAL_ACCU_2].totalAh);
    DEBUG_LOGF("[loadAhFromEEPROM] Accu 2 Ah (%f) geladen van adres %d", ina.channels[KANAAL_ACCU_2].totalAh, addr);
    EEPROM.end();
}

void EEPROM_Manager::saveCapacityToEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int addr = ADDR_CAPACITY;
    EEPROM.put(addr, ina.channels[KANAAL_ACCU_1].batteryCapacityAh);
    DEBUG_LOGF("[saveCapacityToEEPROM] Accu 1 capaciteit (%f Ah) opgeslagen op adres %d", ina.channels[KANAAL_ACCU_1].batteryCapacityAh, addr);
    addr += sizeof(float);
    EEPROM.put(addr, ina.channels[KANAAL_ACCU_2].batteryCapacityAh);
    DEBUG_LOGF("[saveCapacityToEEPROM] Accu 2 capaciteit (%f Ah) opgeslagen op adres %d", ina.channels[KANAAL_ACCU_2].batteryCapacityAh, addr);
    EEPROM.commit();
    EEPROM.end();
}

void EEPROM_Manager::loadCapacityFromEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    int addr = ADDR_CAPACITY;
    EEPROM.get(addr, ina.channels[KANAAL_ACCU_1].batteryCapacityAh);
    DEBUG_LOGF("[loadCapacityFromEEPROM] Accu 1 capaciteit (%f Ah) geladen van adres %d", ina.channels[KANAAL_ACCU_1].batteryCapacityAh, addr);
    addr += sizeof(float);
    EEPROM.get(addr, ina.channels[KANAAL_ACCU_2].batteryCapacityAh);
    DEBUG_LOGF("[loadCapacityFromEEPROM] Accu 2 capaciteit (%f Ah) geladen van adres %d", ina.channels[KANAAL_ACCU_2].batteryCapacityAh, addr);
    EEPROM.end();

    if (ina.channels[KANAAL_ACCU_1].batteryCapacityAh <= 0.1f)
    {
        ina.channels[KANAAL_ACCU_1].batteryCapacityAh = 200.0f;
        DEBUG_LOG("[loadCapacityFromEEPROM] Accu 1 capaciteit hersteld naar standaard (200.0 Ah)");
    }
    if (ina.channels[KANAAL_ACCU_2].batteryCapacityAh <= 0.1f)
    {
        ina.channels[KANAAL_ACCU_2].batteryCapacityAh = 100.0f;
        DEBUG_LOG("[loadCapacityFromEEPROM] Accu 2 capaciteit hersteld naar standaard (100.0 Ah)");
    }
}

void EEPROM_Manager::factoryResetEEPROM()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.write(i, 0xFF);
    }
    EEPROM.commit();
    EEPROM.end();

    DEBUG_LOG("[factoryResetEEPROM] EEPROM gewist naar 0xFF");

    ina.setDefaultCalibration(); // <-- Correcte aanroep
    saveCalibrationToEEPROM();
    saveAhToEEPROM();
    saveCapacityToEEPROM();

    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        debugSettings.moduleEnabled[i] = false;
    }
    saveDebugSettingsToEEPROM();

    DEBUG_LOG("[factoryResetEEPROM] Factory reset voltooid en standaardinstellingen opgeslagen.");
}

bool EEPROM_Manager::isEEPROMVersionValid()
{
    EEPROM.begin(EEPROM_SIZE);
    int version;
    EEPROM.get(ADDR_VERSION, version);
    EEPROM.end();
    return (version == EEPROM_VERSION);
}

void EEPROM_Manager::loadDefaultsIfNeeded()
{
    INA3221_Module &ina = INA3221_Module::getInstance();

    if (!isEEPROMVersionValid())
    {
        DEBUG_LOGF("[loadDefaultsIfNeeded] Ongeldige EEPROM versie, laden standaardwaarden...");

        ina.setDefaultCalibration(); // <-- Correcte aanroep

        ina.channels[KANAAL_ACCU_1].batteryCapacityAh = 200.0f;
        ina.channels[KANAAL_ACCU_2].batteryCapacityAh = 100.0f;

        ina.channels[KANAAL_ACCU_1].totalAh = 0.0f;
        ina.channels[KANAAL_ACCU_2].totalAh = 0.0f;

        EEPROM.begin(EEPROM_SIZE);
        EEPROM.put(ADDR_VERSION, EEPROM_VERSION);
        EEPROM.end();

        saveCalibrationToEEPROM();
        saveCapacityToEEPROM();
        saveAhToEEPROM();

        for (int i = 0; i < MAX_DEBUG_MODULES; i++)
        {
            debugSettings.moduleEnabled[i] = false;
        }
        saveDebugSettingsToEEPROM();

        DEBUG_LOG("[loadDefaultsIfNeeded] Standaard EEPROM instellingen geladen en opgeslagen.");
    }
    else
    {
        DEBUG_LOGF("[loadDefaultsIfNeeded] EEPROM versie OK (%d)", EEPROM_VERSION);
    }
}

void EEPROM_Manager::saveLanguageToEEPROM(const String &langCode)
{
    EEPROM.begin(EEPROM_SIZE);
    char buffer[LANG_CODE_LENGTH] = {0};
    langCode.toCharArray(buffer, LANG_CODE_LENGTH);
    EEPROM.put(ADDR_LANGUAGE, buffer);
    EEPROM.commit();
    EEPROM.end();
    DEBUG_LOGF("[saveLanguageToEEPROM] Taalcode '%s' opgeslagen", buffer);
}

String EEPROM_Manager::loadLanguageFromEEPROM()
{
    EEPROM.begin(EEPROM_SIZE);
    char buffer[LANG_CODE_LENGTH] = {0};
    EEPROM.get(ADDR_LANGUAGE, buffer);
    EEPROM.end();
    String lang(buffer);
    lang.trim();
    if (lang.length() == 0)
    {
        lang = "nl";
    }
    DEBUG_LOGF("[loadLanguageFromEEPROM] Taalcode geladen: '%s'", lang.c_str());
    return lang;
}

void EEPROM_Manager::init()
{
    loadDebugSettingsFromEEPROM();
    loadCalibrationFromEEPROM();
    loadAhFromEEPROM();
    loadCapacityFromEEPROM();
}
