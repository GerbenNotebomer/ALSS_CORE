#include "route_debug_log.h"
#include "../../common/common_includes.h"

//  --  generateDebugLogPage  --  //
String generateDebugLogPage()
{
    String extraCss = R"rawliteral(
<style>
    .flex-container {
        display: flex;
        gap: 20px;
        flex-wrap: wrap;
    }
    .settings-side {
        flex: 1;
        min-width: 250px;
    }
    .log-side {
        flex: 2;
        min-width: 400px;
    }
    .log-content {
        white-space: pre-wrap;
        background-color: #f4f4f4;
        padding: 10px;
        border-radius: 5px;
        height: 400px;
        overflow-y: auto;
    }
        /* Zorg voor nette uitlijning van checkboxen */
    .field-row.checkbox-label {
        display: flex;
        align-items: center;
        gap: 8px;
        margin-bottom: 6px;
    }

    .field-row.checkbox-label label {
        flex: 1;
        display: flex;
        align-items: center;
        gap: 8px;
        font-family: sans-serif;
    }
</style>
)rawliteral";

    // Hoofdinhoud pagina
    String content;

    content += "<fieldset class=\"info-box\">\n";
    content += "  <h1>" + langManager.get("debuglog.title") + "</h1>\n";
    content += "</fieldset>\n";

    content += R"rawliteral(

<div class="flex-container">
    <!-- Instellingen (links) -->
    <div class="settings-side">
        <form method="POST" action="/debuglog/settings">
            <fieldset class="info-box">
                <legend>Debugmodules</legend>
)rawliteral";

    // Checkboxen voor debugmodules
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        bool isEnabled = DebugUtil::isModuleEnabled(moduleNames[i]);
        content += "<div class='field-row checkbox-label'>";
        content += "<label><input type='checkbox' name='" + String(moduleNames[i]) + "'";
        if (isEnabled)
            content += " checked";
        content += "> " + String(moduleNames[i]) + "</label></div>\n";
    }

    content += "</fieldset>\n";
    content += "<div class=\"button-container\">\n";
    content += "  <input type=\"submit\" value=\"" + langManager.get("button.save") + "\">\n";
    content += "  <button type=\"button\" onclick=\"clearLogs()\">" + langManager.get("button.wissen") + "</button>\n";
    content += "</div>\n";
    content += "</form>\n";
    content += "</div>\n";

    content += "<div class=\"log-side\">\n";
    content += "  <fieldset class=\"info-box\">\n";
    content += "    <legend>" + langManager.get("label.aantalregels") + "</legend>\n";
    content += "    <div><span id=\"logCount\">0</span></div>\n";
    content += "  </fieldset>\n";
    content += "  <fieldset class=\"info-box\">\n";
    content += "    <legend>" + langManager.get("label.log") + "</legend>\n";
    content += "    <pre id=\"logContent\" class=\"log-content\">" + langManager.get("label.laden") + "</pre>\n";
    content += "  </fieldset>\n";
    content += "</div>\n";
    content += "</div>\n";

    /*
    content += R"rawliteral(
            </fieldset>
            <div class="button-container">
                <input type="submit" value="📥 Opslaan">
                <button type="button" onclick="clearLogs()">🗑️ Wissen</button>
            </div>
        </form>
    </div>
    <!-- Log (rechts) -->
    <div class="log-side">
        <fieldset class="info-box">
            <legend>Aantal regels</legend>
            <div><span id="logCount">0</span></div>
        </fieldset>
        <fieldset class="info-box">
            <legend>Log</legend>
            <pre id="logContent" class="log-content">Laden...</pre>
        </fieldset>
    </div>
</div>
)rawliteral";
*/

    // Modal HTML met button classes ipv ID's
    /*
    String modalHtml = R"rawliteral(
<div id="confirmModal" class="modal">
  <div class="modal-content">
    <p>Weet je zeker dat je de log wilt wissen?</p>
    <button class="btn-confirm-yes">Ja</button>
    <button class="btn-confirm-no">Nee</button>
  </div>
</div>
)rawliteral";
*/
    String modalHtml;
    modalHtml += "<div id=\"confirmModal\" class=\"modal\">\n";
    modalHtml += "  <div class=\"modal-content\">\n";
    modalHtml += "    <p>" + langManager.get("bevestiging.wisdebuglog.melding") + "</p>\n";
    modalHtml += "    <button class=\"btn-confirm-yes\">" + langManager.get("button.ja") + "</button>\n";
    modalHtml += "    <button class=\"btn-confirm-no\">" + langManager.get("button.nee") + "</button>\n";
    modalHtml += "  </div>\n";
    modalHtml += "</div>\n";

    content += modalHtml;

    // JavaScript met class-selectors ipv ID's
    String extraJs = R"rawliteral(
<script>
    async function fetchLogs() {
        try {
            const response = await fetch('/debuglog/json');
            if (!response.ok) throw new Error('Netwerkfout bij ophalen logs');
            const data = await response.json();
            document.getElementById('logCount').textContent = data.logCount || 0;
            let logContent = '[log is leeg]';
            if (data.logs) {
                if (Array.isArray(data.logs)) {
                    logContent = data.logs.join('\n');
                } else if (typeof data.logs === 'string') {
                    logContent = data.logs;
                }
            }
            document.getElementById('logContent').textContent = logContent;
        } catch(e) {
            document.getElementById('logContent').textContent = '[Fout bij ophalen log]';
            console.error(e);
        }
    }

    function clearLogs() {
        const modal = document.getElementById('confirmModal');
        modal.style.display = 'flex';
        const yesBtn = modal.querySelector('.btn-confirm-yes');
        const noBtn = modal.querySelector('.btn-confirm-no');

        yesBtn.onclick = async () => {
            modal.style.display = 'none';
            try {
                const response = await fetch('/debuglog/clear', { method: 'POST' });
                if (!response.ok) throw new Error('Fout bij wissen log');
                await fetchLogs();
            } catch (e) {
                alert("Kon log niet wissen.");
                console.error(e);
            }
        };

        noBtn.onclick = () => {
            modal.style.display = 'none';
        };
    }

    setInterval(fetchLogs, 3000);
    window.onload = fetchLogs;
</script>
)rawliteral";

    // Render de volledige pagina (vergeet sharedCss() niet in renderPage toe te voegen)
    return HtmlUtils::renderPage(content, "Debug Log Viewer", "/debuglog/view", extraCss, extraJs);
}

//  --  handleDebugLogPage  --  //
void RouteDebugLog::handleDebugLogPage(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", generateDebugLogPage());
}

//  --  handleDebugLogJson  --  //
void RouteDebugLog::handleDebugLogJson(AsyncWebServerRequest *request)
{
    DynamicJsonDocument doc(8192);

    JsonArray logs = doc.createNestedArray("logs");

    for (const String &line : DebugUtil::logBuffer)
    {
        logs.add(line);
    }

    doc["logCount"] = DebugUtil::logBuffer.size();

    String response;
    serializeJson(doc, response);

    request->send(200, "application/json", response);
}

//  --  handleDebugSettingsPost --  //
void RouteDebugLog::handleDebugSettingsPost(AsyncWebServerRequest *request)
{
    for (int i = 0; i < MAX_DEBUG_MODULES; i++)
    {
        bool enabled = request->hasParam(moduleNames[i], true); // POST params
        DebugUtil::setModuleEnabled(moduleNames[i], enabled);
    }

    EEPROM_Manager::saveDebugSettingsToEEPROM(); // Optioneel

    // Redirect terug naar de logpagina
    request->redirect("/debuglog/view");
}

//  --  registerRoutes --  //
void RouteDebugLog::registerRoutes(AsyncWebServer &server)
{
    server.on("/debuglog/view", HTTP_GET, RouteDebugLog::handleDebugLogPage);
    server.on("/debuglog/json", HTTP_GET, RouteDebugLog::handleDebugLogJson);
    server.on("/debuglog/settings", HTTP_POST, RouteDebugLog::handleDebugSettingsPost);
    server.on("/debuglog/clear", HTTP_POST, RouteDebugLog::handleDebugClearPost); // ← toegevoegd
}

//  --  handleDebugClearPost  --  //
void RouteDebugLog::handleDebugClearPost(AsyncWebServerRequest *request)
{
    DebugUtil::clearLogBuffer(); // Zorg dat dit bestaat in DebugUtil.cpp/h
    request->send(200, "text/plain", "Log gewist");
}
