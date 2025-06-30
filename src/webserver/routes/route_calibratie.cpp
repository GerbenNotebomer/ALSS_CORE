//  --  generateCalibrationPage  --  //
#include "route_calibratie.h"
#include "../route_base.h"
#include "../route_hardware.h"

extern LanguageManager langManager;

//  --  generateCalibrationPage  --  //
String generateCalibrationPage(const String &msg)
{
    String content;

    content += "<fieldset class=\"info-box\">\n";
    content += "  <h1>" + langManager.get("kalibratie.title") + "</h1>\n";
    content += "</fieldset>\n";

    if (msg.length() > 0)
    {
        content += "<div id=\"statusModal\" class=\"modal\" style=\"display:none; align-items:center; justify-content:center;\">";
        content += "  <div class=\"modal-content\">";
        content += "    <p id=\"statusMessage\">" + msg + "</p>";
        content += "    <button onclick=\"closeStatusModal()\">" + langManager.get("button.sluiten") + "</button>";
        content += "  </div>";
        content += "</div>";
    }

    content += "<form method=\"POST\" action=\"/calibratie\">\n";

    const int gebruikteKanalen[] = {KANAAL_MPPT, KANAAL_ACCU_1, KANAAL_ACCU_2};
    const int aantalKanalen = sizeof(gebruikteKanalen) / sizeof(gebruikteKanalen[0]);

    for (int idx = 0; idx < aantalKanalen; idx++)
    {
        int i = gebruikteKanalen[idx];
        INA3221_Module &ina = INA3221_Module::getInstance();
        ChannelSettings &cfg = ina.channelSettings[i];
        // ChannelSettings &cfg = channelSettings[i];
        String prefix = "ch" + String(i) + "_";

        content += "<fieldset class=\"info-box\"><legend>" + String(cfg.name) + "</legend>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "mvOffset\">mV Offset</label>";
        content += "<input type=\"number\" step=\"0.0001\" name=\"" + prefix + "mvOffset\" value=\"" + String(cfg.mvOffset, 5) + "\" required></div>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "mvGain\">mV Gain</label>";
        content += "<input type=\"number\" step=\"0.0001\" name=\"" + prefix + "mvGain\" value=\"" + String(cfg.mvGain, 5) + "\" required></div>\n";

        content += "<div class=\"field-row checkbox-label\">";
        content += "<label><input type=\"checkbox\" name=\"" + prefix + "invertCurrent\" " + (cfg.invertCurrent ? "checked" : "") + "> Inverteer stroom</label></div>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "currentCorrection\">" + langManager.get("label.stroom_correctie") + "</label>";
        content += "<input type=\"number\" step=\"0.0001\" name=\"" + prefix + "currentCorrection\" value=\"" + String(cfg.currentCorrection, 5) + "\" required></div>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "voltageCorrection\">" + langManager.get("label.spanning_correctie") + "</label>";
        content += "<input type=\"number\" step=\"0.0001\" name=\"" + prefix + "voltageCorrection\" value=\"" + String(cfg.voltageCorrection, 5) + "\" required></div>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "shuntVoltage_mV\">Shunt Spanning (mV)</label>";
        content += "<input type=\"number\" step=\"0.01\" name=\"" + prefix + "shuntVoltage_mV\" value=\"" + String(cfg.shuntVoltage_mV, 2) + "\" required></div>\n";

        content += "<div class=\"field-row\"><label for=\"" + prefix + "shuntCurrent_A\">Shunt Stroom (A)</label>";
        content += "<input type=\"number\" step=\"0.001\" name=\"" + prefix + "shuntCurrent_A\" value=\"" + String(cfg.shuntCurrent_A, 3) + "\" required></div>\n";

        if (i == KANAAL_ACCU_1 || i == KANAAL_ACCU_2)
        {
            INA3221_Module &ina = INA3221_Module::getInstance();
            ChannelSettings &cfg = ina.channelSettings[i];
            float ah = ina.channels[i].batteryCapacityAh;

            // float ah = channels[i].batteryCapacityAh;
            content += "<div class=\"field-row\"><label for=\"" + prefix + "batteryCapacityAh\">BatteryCapacityAh</label>";
            content += "<input type=\"number\" step=\"0.1\" name=\"" + prefix + "batteryCapacityAh\" value=\"" + String(ah, 1) + "\" required></div>\n";
        }

        content += "</fieldset>\n";
    }

    content += "<fieldset class=\"info-box\">\n";
    content += "  <legend>" + langManager.get("section.rekenhulp") + "</legend>\n";
    content += "  <div class=\"field-row\">\n";
    content += "    <label for=\"kanaal\">" + langManager.get("label.kies_kanaal") + "</label>\n";
    content += "    <select id=\"kanaal\" name=\"kanaal\">\n";
    content += "      <option value=\"1\">Kanaal 1</option>\n";
    content += "      <option value=\"2\">Kanaal 2</option>\n";
    content += "      <option value=\"3\">Kanaal 3</option>\n";
    content += "    </select>\n";
    content += "  </div>\n";
    content += "  <p><strong>" + langManager.get("label.spanning_ina") + "</strong></p>\n";
    content += "  <p>Spanning (V): <span id=\"gemetenVolt\">-</span></p>\n";
    content += "  <p>Stroom (A): <span id=\"gemetenAmpere\">-</span></p>\n";
    content += "  <div class=\"field-row\">\n";
    content += "    <label for=\"multimeterVolt\">" + langManager.get("label.multimeter_volt") + "</label>\n";
    content += "    <input type=\"number\" step=\"0.01\" id=\"multimeterVolt\">\n";
    content += "  </div>\n";
    content += "  <div class=\"field-row\">\n";
    content += "    <label for=\"multimeterAmpere\">" + langManager.get("label.multimeter_ampere") + "</label>\n";
    content += "    <input type=\"number\" step=\"0.001\" id=\"multimeterAmpere\">\n";
    content += "  </div>\n";
    content += "  <p><strong>" + langManager.get("label.correctie_voorgesteld") + "</strong></p>\n";
    content += "  <p>" + langManager.get("label.spanning_correctie") + ": <span id=\"voltCorr\">-</span></p>\n";
    content += "  <p>" + langManager.get("label.stroom_correctie") + ": <span id=\"ampCorr\">-</span></p>\n";
    content += "  <button type=\"button\" onclick=\"berekenCorrectie()\">" + langManager.get("button.bereken") + "</button>\n";
    content += "</fieldset>\n";

    content += "<div class=\"button-container\">\n";
    content += "  <input type=\"submit\" value=\"" + langManager.get("button.save") + "\">\n";
    content += "</div>\n";
    content += "<div class=\"button-container\">\n";
    content += "  <button type=\"button\" onclick=\"openResetModal()\">" + langManager.get("button.reset") + "</button>\n";
    content += "</div>\n";
    content += "</form>\n";

    // === Modals (alert en reset)
    content += "<div id=\"alertModal\" class=\"modal\" style=\"display:none; align-items:center; justify-content:center;\">\n";
    content += "  <div class=\"modal-content\">\n";
    content += "    <p id=\"alertMessage\"></p>\n";
    content += "    <button class=\"btn-confirm-no\">" + langManager.get("modal.alert.ok") + "</button>\n";
    content += "  </div>\n";
    content += "</div>\n";

    content += "<div id=\"resetModal\" class=\"modal\" style=\"display:none; align-items:center; justify-content:center;\">\n";
    content += "  <div class=\"modal-content\">\n";
    content += "    <p>" + langManager.get("modal.reset.title") + "</p>\n";
    content += "    <div class=\"modal-button-group\">\n";
    content += "      <button class=\"btn-confirm-yes\" onclick=\"confirmFactoryReset()\">" + langManager.get("button.ja") + "</button>\n";
    content += "      <button class=\"btn-confirm-no\" onclick=\"closeResetModal()\">" + langManager.get("button.nee") + "</button>\n";
    content += "    </div>\n";
    content += "  </div>\n";
    content += "</div>\n";

    // === JavaScript blijft hetzelfde, behalve foutmelding
    String extraJs = R"rawliteral(
<script>
let kanaalData = {};

async function fetchKanaalData() {
    try {
        const resp = await fetch('/data.json');
        const data = await resp.json();
        kanaalData = {};
        let i = 1;
        data.channels.forEach(ch => {
            if ('voltage' in ch && 'current' in ch) {
                kanaalData[i] = {
                    voltage: ch.voltage,
                    current: ch.current
                };
                i++;
            }
        });
        updateWaarden();
    } catch (e) {
        console.error(e);
    }
}

function updateWaarden() {
    const kanaal = document.getElementById('kanaal').value;
    const v = kanaalData[kanaal]?.voltage?.toFixed(2) ?? '-';
    const a = kanaalData[kanaal]?.current?.toFixed(3) ?? '-';
    document.getElementById('gemetenVolt').textContent = v;
    document.getElementById('gemetenAmpere').textContent = a;
}

document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('kanaal').addEventListener('change', updateWaarden);
    fetchKanaalData();
    setInterval(fetchKanaalData, 2000);

    const modal = document.getElementById('statusModal');
    if (modal) {
        modal.style.display = 'flex';
        setTimeout(() => {
            modal.style.display = 'none';
        }, 5000);
    }
});

function showAlert(message) {
    const modal = document.getElementById('alertModal');
    document.getElementById('alertMessage').textContent = message;
    modal.style.display = 'flex';
    modal.querySelector('.btn-confirm-no').onclick = () => {
        modal.style.display = 'none';
    };
}

function closeStatusModal() {
    const modal = document.getElementById('statusModal');
    if (modal) modal.style.display = 'none';
}

function berekenCorrectie() {
    const kanaal = document.getElementById('kanaal').value;
    const gemVolt = kanaalData[kanaal]?.voltage;
    const gemAmp = kanaalData[kanaal]?.current;
    const multVolt = parseFloat(document.getElementById('multimeterVolt').value);
    const multAmp = parseFloat(document.getElementById('multimeterAmpere').value);

    if (isNaN(multVolt) || isNaN(multAmp) || !gemVolt || !gemAmp) {
        showAlert(")rawliteral" +
                     langManager.get("alert.fields_required") + R"rawliteral(");
        return;
    }

    const voltCorr = multVolt / gemVolt;
    const ampCorr = multAmp / gemAmp;

    document.getElementById('voltCorr').textContent = voltCorr.toFixed(5);
    document.getElementById('ampCorr').textContent = ampCorr.toFixed(5);
}

function openResetModal() {
    document.getElementById('resetModal').style.display = 'flex';
}

function closeResetModal() {
    document.getElementById('resetModal').style.display = 'none';
}

function confirmFactoryReset() {
    fetch('/calibratie/reset', { method: 'POST' })
        .then(() => {
            window.location.href = "/calibratie?msg=)rawliteral" +
                     langManager.get("reset.success") + R"rawliteral(";
        })
        .catch(err => {
            console.error("Reset mislukt:", err);
            showAlert("Reset mislukt, probeer opnieuw.");
        });
}
</script>
)rawliteral";

    return HtmlUtils::renderPage(content, langManager.get("kalibratie.title"), "/calibratie", "", extraJs);
}

//  --  getArgFloat  --  //
bool getArgFloat(AsyncWebServerRequest *request, int ch, const char *suffix, float &outVal)
{
    char param[32];
    snprintf(param, sizeof(param), "ch%d_%s", ch, suffix);

    if (request->hasParam(param, true)) // 'true' = POST/body
    {
        outVal = request->getParam(param, true)->value().toFloat();
        return true;
    }
    return false;
}

//  --  getArgBool  --  //
bool getArgBool(AsyncWebServerRequest *request, int ch, const char *suffix, bool &outVal)
{
    char param[32];
    snprintf(param, sizeof(param), "ch%d_%s", ch, suffix);

    if (request->hasParam(param, true))
    {
        // Checkbox = aanwezig => "on", niet aanwezig => niets
        outVal = (request->getParam(param, true)->value() == "on");
        return true;
    }

    outVal = false;
    return false;
}

//  --  handleCalibratie  --  //
void RouteCalibration::handleCalibratie(AsyncWebServerRequest *request)
{
    String msg;
    if (request->hasParam("msg"))
        msg = request->getParam("msg")->value();

    request->send(200, "text/html", generateCalibrationPage(msg));
}

//  --  registerRoutes  --  //
void RouteCalibration::registerRoutes(AsyncWebServer &server)
{
    server.on("/calibratie/reset", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        INA3221_Module::getInstance().setDefaultCalibration();    // Herstel fabrieksinstellingen
        EEPROM_Manager::saveCalibrationToEEPROM();                // Opslaan kalibratie
        EEPROM_Manager::saveCapacityToEEPROM();                   // Opslaan capaciteit
        EEPROM_Manager::saveAhToEEPROM();                         // Reset totalAh -> opslaan
        request->send(200, "text/plain", "Reset OK"); });

    server.on("/calibratie", HTTP_GET, handleCalibratie);

    server.on("/calibratie", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        INA3221_Module &ina = INA3221_Module::getInstance();

        for (int i = 1; i <= NUM_INA_CHANNELS; i++) {
            ChannelSettings &cfg = ina.channelSettings[i];
            float f;

            if (getArgFloat(request, i, "mvOffset", f)) cfg.mvOffset = f;
            if (getArgFloat(request, i, "mvGain", f)) cfg.mvGain = f;
            getArgBool(request, i, "invertCurrent", cfg.invertCurrent);
            if (getArgFloat(request, i, "currentCorrection", f)) cfg.currentCorrection = f;
            if (getArgFloat(request, i, "voltageCorrection", f)) cfg.voltageCorrection = f;
            if (getArgFloat(request, i, "shuntVoltage_mV", f)) cfg.shuntVoltage_mV = f;
            if (getArgFloat(request, i, "shuntCurrent_A", f)) cfg.shuntCurrent_A = f;
        }

        for (int i = 1; i <= NUM_INA_CHANNELS; i++) {
            if (i == KANAAL_ACCU_1 || i == KANAAL_ACCU_2) {
                float f;
                if (getArgFloat(request, i, "batteryCapacityAh", f)) {
                    ina.channels[i].batteryCapacityAh = f;
                }
            }
        }

        EEPROM_Manager::saveCalibrationToEEPROM();
        EEPROM_Manager::saveCapacityToEEPROM();

        // Redirect met msg
        request->redirect("/calibratie?msg=Kalibratie+opgeslagen"); });
}

/*
void RouteCalibration::registerRoutes(AsyncWebServer &server)
{
    server.on("/calibratie/reset", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    setDefaultCalibration();                        // Herstel fabrieksinstellingen
    EEPROM_Manager::saveCalibrationToEEPROM();      // Opslaan kalibratie
    EEPROM_Manager::saveCapacityToEEPROM();         // Opslaan capaciteit
    EEPROM_Manager::saveAhToEEPROM();               // Reset totalAh -> opslaan
    request->send(200, "text/plain", "Reset OK"); });

    server.on("/calibratie", HTTP_GET, handleCalibratie);

    server.on("/calibratie", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        for (int i = 1; i <= NUM_INA_CHANNELS; i++) {
            ChannelSettings &cfg = channelSettings[i];
            float f;

            if (getArgFloat(request, i, "mvOffset", f)) cfg.mvOffset = f;
            if (getArgFloat(request, i, "mvGain", f)) cfg.mvGain = f;
            getArgBool(request, i, "invertCurrent", cfg.invertCurrent);
            if (getArgFloat(request, i, "currentCorrection", f)) cfg.currentCorrection = f;
            if (getArgFloat(request, i, "voltageCorrection", f)) cfg.voltageCorrection = f;
            if (getArgFloat(request, i, "shuntVoltage_mV", f)) cfg.shuntVoltage_mV = f;
            if (getArgFloat(request, i, "shuntCurrent_A", f)) cfg.shuntCurrent_A = f;
        }

        for (int i = 1; i <= NUM_INA_CHANNELS; i++) {
            if (i == KANAAL_ACCU_1 || i == KANAAL_ACCU_2) {
                float f;
                if (getArgFloat(request, i, "batteryCapacityAh", f)) {
                    channels[i].batteryCapacityAh = f;
                }
            }
        }

        EEPROM_Manager::saveCalibrationToEEPROM();
        EEPROM_Manager::saveCapacityToEEPROM();

        // Redirect met msg
        request->redirect("/calibratie?msg=Kalibratie+opgeslagen"); });
}
*/