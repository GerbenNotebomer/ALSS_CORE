#include "route_weersvoorspelling.h"
#include "../../common/common_includes.h"

//  --  generatePage  --  //
String RouteWeersvoorspelling::generatePage()
{
    String extraCss = R"rawliteral(
        <style>
            .forecast-box, .sensor-box {
                background: #f0f8ff;
                padding: 20px;
                border-radius: 10px;
                font-family: monospace;
                font-size: 1.1em;
                margin-bottom: 20px;
            }
            .sensor-box table {
                width: 100%;
                border-collapse: collapse;
            }
            .sensor-box th, .sensor-box td {
                padding: 8px;
                text-align: left;
            }
            .sensor-box tr:nth-child(even) {
                background-color: #e8f0fe;
            }
            progress {
                appearance: none;
                width: 100%;
                height: 25px;
                border-radius: 10px;
            }
            progress::-webkit-progress-bar {
                background-color: #eee;
                border-radius: 10px;
            }
            progress::-webkit-progress-value {
                background-color: #4caf50;
                border-radius: 10px;
            }
        </style>
    )rawliteral";

    String content = R"rawliteral(
        <fieldset class="info-box">

        <legend>🌤️ )rawliteral" +
                     langManager.get("weersvoorspelling.title") + R"rawliteral(</legend>

        <div class="sensor-box">
            <table>
                <tr><th>🌡️ )rawliteral" +
                     langManager.get("dashboard.temperature_aht") + R"rawliteral(</th><td id="ahtTemp">...</td></tr>
                <tr><th>💧 )rawliteral" +
                     langManager.get("dashboard.humidity_aht") + R"rawliteral(</th><td id="ahtHumidity">...</td></tr>
                <tr><th>🌡️ )rawliteral" +
                     langManager.get("dashboard.temperature_bmp") + R"rawliteral(</th><td id="bmpTemp">...</td></tr>
                <tr><th>📈 )rawliteral" +
                     langManager.get("dashboard.raw_pressure") + R"rawliteral(</th><td id="rawPressure">...</td></tr>
                <tr><th>🌍 )rawliteral" +
                     langManager.get("dashboard.sea_pressure") + R"rawliteral(</th><td id="seaPressure">...</td></tr>
            </table>
        </div>

        <div class="forecast-box" id="forecastBox">🔄 )rawliteral" +
                     langManager.get("weersvoorspelling.loading") + R"rawliteral(</div>

        <div style="margin-top: 20px;">
            <label for="measurementBar">📊 )rawliteral" +
                     langManager.get("weersvoorspelling.measurements_label") + R"rawliteral(</label><br>
            <progress id="measurementBar" max="10" value="0"></progress>
            <div id="measurementText" style="font-family: monospace; margin-top: 5px;">)rawliteral" +
                     langManager.get("weersvoorspelling.measurements_loading") + R"rawliteral(</div>
        </div>
 
        </fieldset>
    )rawliteral";

    String extraJs = R"rawliteral(
        <script>
            async function updateForecast() {
                try {
                    const response = await fetch('/api/weersvoorspelling');
                    const data = await response.json();

                    document.getElementById('ahtTemp').textContent = data.ahtTemp.toFixed(1) + ' °C';
                    document.getElementById('ahtHumidity').textContent = data.ahtHumidity.toFixed(1) + ' %';
                    document.getElementById('bmpTemp').textContent = data.bmpTemp.toFixed(1) + ' °C';
                    document.getElementById('rawPressure').textContent = data.rawPressure.toFixed(1) + ' hPa';
                    document.getElementById('seaPressure').textContent = data.seaPressure.toFixed(1) + ' hPa';
                    document.getElementById('forecastBox').textContent = '🔮 ' + data.forecast;

                    // Voortgangsbalk updaten
                    const progressBar = document.getElementById('measurementBar');
                    progressBar.max = data.measurementsNeeded;
                    progressBar.value = data.measurementsDone;

                    const text = `${data.measurementsDone} / ${data.measurementsNeeded} metingen`;
                    document.getElementById('measurementText').textContent = text;
                } catch (e) {
                    console.error('Fout bij ophalen van voorspelling:', e);
                    document.getElementById('forecastBox').textContent = '⚠️ Fout bij ophalen gegevens';
                    document.getElementById('measurementText').textContent = '⚠️ Fout bij metingen ophalen';
                }
            }

            updateForecast();
            setInterval(updateForecast, 30000);
        </script>
    )rawliteral";

    return HtmlUtils::renderPage(content, langManager.get("weersvoorspelling.title"), "/weersvoorspelling", extraCss, extraJs);
}

//  --  handleApi  --  //
void RouteWeersvoorspelling::handleApi(AsyncWebServerRequest *request)
{
    auto &sensorModule = AHT20BMP280_Module::getInstance();
    const SensorData &data = sensorModule.readSensors();
    String forecast = sensorModule.calculateForecast();

    int done = sensorModule.getMeasurementCount();
    int needed = sensorModule.getRequiredMeasurementCount();

    String json = "{";
    json += "\"ahtTemp\":" + String(data.ahtTemperature, 1) + ",";
    json += "\"ahtHumidity\":" + String(data.ahtHumidity, 1) + ",";
    json += "\"bmpTemp\":" + String(data.bmpTemperature, 1) + ",";
    json += "\"rawPressure\":" + String(data.rawPressure, 1) + ",";
    json += "\"seaPressure\":" + String(data.pressure, 1) + ",";
    json += "\"forecast\":\"" + forecast + "\",";
    json += "\"measurementsDone\":" + String(done) + ",";
    json += "\"measurementsNeeded\":" + String(needed);
    json += "}";

    request->send(200, "application/json", json);
}

//  --  registerRoutes  --  //
void RouteWeersvoorspelling::registerRoutes(AsyncWebServer &server)
{
    server.on("/weersvoorspelling", HTTP_GET, handlePage);
    server.on("/api/weersvoorspelling", HTTP_GET, handleApi);
}

//  --  handlePage  --  //
void RouteWeersvoorspelling::handlePage(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", generatePage());
}
