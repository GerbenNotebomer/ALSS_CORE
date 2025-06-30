#include "route_dashboard.h"
#include "../../common/common_includes.h"

//  --  generateDashboardPage  --  //
String generateDashboardPage()
{
    String content = "<fieldset class=\"info-box\">\n";
    content += "  <div class=\"dashboard-header\">\n";
    content += "    <h1>" + langManager.get("dashboard.title") + "</h1>\n";
    content += "    <div id=\"timestampDisplay\">" + langManager.get("dashboard.loading") + "</div>\n";
    content += "  </div>\n";
    content += "</fieldset>\n";

    content += R"rawliteral(
    <div class="info-box">
        <table>
            <thead>
                <tr>
                    <th>)rawliteral" +
               langManager.get("dashboard.channel") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.voltage") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.current") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.power") + R"rawliteral(</th>
                </tr>
            </thead>
            <tbody id="dataBody">
                <tr><td colspan="4">)rawliteral" +
               langManager.get("dashboard.loading") + R"rawliteral(</td></tr>
            </tbody>
        </table>
    </div>
    )rawliteral";

    content += "<div class=\"info-box\">\n";
    content += "  <div class=\"header-flex\">\n";
    content += "    <h2>" + langManager.get("dashboard.batteries") + "</h2>\n";
    content += "    <select id=\"accuSelect\" onchange=\"fetchData()\">\n";
    content += "      <option value=\"1,2\">" + langManager.get("dashboard.battery") + " 1 & 2</option>\n";
    content += "      <option value=\"1\">" + langManager.get("dashboard.battery") + " 1</option>\n";
    content += "      <option value=\"2\">" + langManager.get("dashboard.battery") + " 2</option>\n";
    content += "    </select>\n";
    content += "  </div>\n";

    content += R"rawliteral(
        <table>
            <thead>
                <tr>
                    <th>)rawliteral" +
               langManager.get("dashboard.battery") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.status") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.soc") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.remaining_time") + R"rawliteral(</th>
                    <th>)rawliteral" +
               langManager.get("dashboard.charge_bar") + R"rawliteral(</th>
                </tr>
            </thead>
            <tbody id="batteryTableBody">
                <tr><td colspan="5">)rawliteral" +
               langManager.get("dashboard.loading") + R"rawliteral(</td></tr>
            </tbody>
        </table>
    </div>
    )rawliteral";

    // Taaldata voor gebruik in JavaScript
    String translationsJs = "<script>\n";
    translationsJs += "const translations = {\n";
    translationsJs += "  charging: \"" + langManager.get("dashboard.state.charging") + "\",\n";
    translationsJs += "  full: \"" + langManager.get("dashboard.state.full") + "\",\n";
    translationsJs += "  discharging: \"" + langManager.get("dashboard.state.discharging") + "\",\n";
    translationsJs += "  idle: \"" + langManager.get("dashboard.state.idle") + "\",\n";
    translationsJs += "  unknown: \"" + langManager.get("dashboard.state.unknown") + "\",\n";
    translationsJs += "  fetch_error: \"" + langManager.get("dashboard.fetch_error") + "\"\n";
    translationsJs += "};\n";
    translationsJs += "</script>\n";

    String extraCss = R"rawliteral(
<style>
.dashboard-header {
    display: flex;
    justify-content: space-between;
    align-items: baseline;
    margin-bottom: 10px;
}

.dashboard-header h1 {
    margin: 0;
    font-size: 1.8em;
}

#timestampDisplay {
    font-size: 1em;
    color: #666;
    font-style: italic;
}
</style>
)rawliteral";

    String extraJs = R"rawliteral(
<script>
async function fetchData() {
    try {
        const select = document.getElementById('accuSelect');
        const accuFilter = select.value;
        localStorage.setItem("selectedAccu", accuFilter);

        const resp = await fetch('/data.json?accu=' + encodeURIComponent(accuFilter));
        const data = await resp.json();

        document.getElementById('timestampDisplay').innerText = `${data.dag} – ${data.datetime}`;

        const tbody = document.getElementById('dataBody');
        tbody.innerHTML = '';
        data.channels.forEach(ch => {
            if ('current' in ch) {
                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td>${ch.name}</td>
                    <td>${ch.voltage.toFixed(2)}</td>
                    <td>${ch.current.toFixed(3)}</td>
                    <td>${ch.power.toFixed(2)}</td>`;
                tbody.appendChild(tr);
            }
        });

        const batteryTable = document.getElementById('batteryTableBody');
        batteryTable.innerHTML = '';
        if (data.batteries && data.batteries.length > 0) {
            const filter = accuFilter.split(',').map(Number);
            data.batteries.forEach((bat, index) => {
                const accuNummer = index + 1;
                if (!filter.includes(accuNummer)) return;

                const iconMap = {
                    "Laden": "🔋 " + translations.charging,
                    "Vol": "✅ " + translations.full,
                    "Ontladen": "⚡ " + translations.discharging,
                    "Rust": "🛑 " + translations.idle
                };
                const state = iconMap[bat.stateText] || translations.unknown;

                const statusColors = {
                    "Laden": "#2196f3",
                    "Ontladen": "#ff9800",
                    "Rust": "#9e9e9e",
                    "Vol": "#4caf50"
                };
                const kleurStatus = statusColors[bat.stateText] || "#000";
                const soc = `${bat.stateOfCharge.toFixed(2)}%`;

                const tijd = (bat.stateText === "Rust" || bat.days < 0)
                    ? `<strong>&#8734;</strong>`
                    : `${bat.days}d ${bat.hours}u ${bat.minutes}m`;

                const kleurSoc =
                    bat.stateOfCharge < 20 ? "#f44336" :
                    bat.stateOfCharge < 50 ? "#ff9800" : "#4caf50";

                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td>Accu ${accuNummer}</td>
                    <td style="color:${kleurStatus}; font-weight:bold;" title="${state}">${state}</td>
                    <td>${soc}</td>
                    <td>${tijd}</td>
                    <td>
                        <div class="battery-bar">
                            <div class="battery-fill" style="width:${bat.stateOfCharge.toFixed(0)}%; background-color:${kleurSoc};"></div>
                        </div>
                    </td>`;
                batteryTable.appendChild(tr);
            });
        } else {
            batteryTable.innerHTML = '<tr><td colspan="5">' + translations.unknown + '</td></tr>';
        }

    } catch (err) {
        console.error("Fout bij ophalen data:", err);
        document.getElementById('timestampDisplay').innerText = "⚠️ " + translations.fetch_error;
    }
}

window.addEventListener('DOMContentLoaded', () => {
    const saved = localStorage.getItem("selectedAccu");
    if (saved) {
        document.getElementById('accuSelect').value = saved;
    }
    fetchData();
    setInterval(fetchData, 1000);
});
</script>
)rawliteral";

    return HtmlUtils::renderPage(content, "Dashboard INA3221", "/dashboard", extraCss, translationsJs + extraJs);
}

//  --  handleDashboard  --  //
void RouteDashboard::handleDashboard(AsyncWebServerRequest *request)
{
    String html = generateDashboardPage();
    request->send(200, "text/html", html);
}

//  --  registerRoutes  --  //
void RouteDashboard::registerRoutes(AsyncWebServer &server)
{
    server.on("/dashboard", HTTP_GET, handleDashboard);
}
