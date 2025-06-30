#include "thing_speak.h"
#include "../common/common_includes.h"
#include "../wifi_sta_ap/wifi_sta_ap.h"
#include "../hardware/aht20_bmp280/aht20_bmp280_module.h"
#include "../hardware/ina3221/ina3221_module.h"

ThingSpeak thingSpeak;

ThingSpeak::ThingSpeak()
    : lastThingSpeakUpdate(0)
{
}

void ThingSpeak::begin()
{
    DEBUG_LOG("[ThingSpeak] Initialisatie voltooid");
    // Hier kun je in de toekomst parameters of status laden
}

void ThingSpeak::update(unsigned long now)
{
    if (now - lastThingSpeakUpdate >= Timers::thingSpeak_UpdateInterval)
    {
        lastThingSpeakUpdate = now;
        sendToThingSpeak();
    }
}

void ThingSpeak::sendToThingSpeak()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        DEBUG_LOGF("[sendToThingSpeak] WiFi niet verbonden");
        return;
    }

    WiFiClient client;
    HTTPClient http;

    String url = "http://api.thingspeak.com/update?api_key=" + Parameters::thingSpeak_ApiKey;
    auto &ina = INA3221_Module::getInstance();

    String f1 = String(ina.channels[1].filteredBusVoltage, 2);
    String f2 = String(ina.channels[1].filteredCurrent_A, 3);
    String f3 = String(ina.channels[1].filteredPower_W, 2);

    String f4 = String(ina.channels[2].filteredBusVoltage, 2);
    String f5 = String(ina.channels[2].filteredCurrent_A, 3);
    String f6 = String(ina.channels[2].filteredPower_W, 2);

    /*
    String f1 = String(channels[1].filteredBusVoltage, 2);
    String f2 = String(channels[1].filteredCurrent_A, 3);
    String f3 = String(channels[1].filteredPower_W, 2);

    String f4 = String(channels[2].filteredBusVoltage, 2);
    String f5 = String(channels[2].filteredCurrent_A, 3);
    String f6 = String(channels[2].filteredPower_W, 2);
*/

    AHT20BMP280_Module &sensor = AHT20BMP280_Module::getInstance();
    const SensorData &sensorData = sensor.readSensors();

    String f7 = String(sensorData.ahtTemperature, 2);
    String f8 = String(sensorData.ahtHumidity, 2);

    url += "&field1=" + f1;
    url += "&field2=" + f2;
    url += "&field3=" + f3;
    url += "&field4=" + f4;
    url += "&field5=" + f5;
    url += "&field6=" + f6;
    url += "&field7=" + f7;
    url += "&field8=" + f8;

    DEBUG_LOGF("[sendToThingSpeak] field1 (Voltage Ch1): %s V", f1.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field2 (Current Ch1): %s A", f2.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field3 (Power Ch1): %s W", f3.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field4 (Voltage Ch2): %s V", f4.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field5 (Current Ch2): %s A", f5.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field6 (Power Ch2): %s W", f6.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field7 (Temp AHT20): %s °C", f7.c_str());
    DEBUG_LOGF("[sendToThingSpeak] field8 (Humidity AHT20): %s %%", f8.c_str());

    DEBUG_LOGF("[sendToThingSpeak] URL: %s", url.c_str());

    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        DEBUG_LOGF("[sendToThingSpeak] Response code: %d, payload: %s", httpCode, payload.c_str());
    }
    else
    {
        DEBUG_LOGF("[sendToThingSpeak] Fout bij ThingSpeak: %s", http.errorToString(httpCode).c_str());
    }
    http.end();
}
