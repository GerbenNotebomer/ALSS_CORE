#include "route_meters.h"
#include "../route_base.h"

//  --  generateSingleMeterSvg  --  //
static String generateSingleMeterSvg(
    const String &channel,
    const String &field,
    float minValue,
    float maxValue,
    const String &title,
    const std::vector<std::pair<int, String>> &zones)
{
  String prefix = channel;
  prefix.replace(" ", "_");
  prefix += "_" + field;

  String content;

  content += R"rawliteral(
<div class="single-meter-wrapper" style="border:1px solid #aaa; border-radius:8px; padding:10px 15px; margin:10px; max-width:260px; background:#fff; box-shadow:0 2px 5px rgba(0,0,0,0.1);">
  <div class="meter-title" style="text-align:center; font-weight:bold; font-size:1.1em; margin-bottom:8px;">)rawliteral" +
             title + R"rawliteral(</div>
  <div class="meter-container" style="max-width:240px; margin:auto;">
)rawliteral";

  content += R"rawliteral(
  <svg width="100%" viewBox="0 0 240 140" style="overflow: visible;">
    <defs>
      <linearGradient id="colorGradient)rawliteral" +
             prefix + R"rawliteral(" x1="0%" y1="0%" x2="100%" y2="0%">
)rawliteral";

  for (const auto &zone : zones)
  {
    content += "<stop offset=\"" + String(zone.first) + "%\" stop-color=\"" + zone.second + "\"/>\n";
  }

  content += R"rawliteral(
      </linearGradient>
      <linearGradient id="widthMask)rawliteral" +
             prefix + R"rawliteral(" x1="0" y1="0" x2="100%" y2="0%">
        <stop offset="0%" stop-color="white" stop-opacity="0.25" />
        <stop offset="100%" stop-color="white" stop-opacity="1" />
      </linearGradient>
      <mask id="fadeMask)rawliteral" +
             prefix + R"rawliteral(">
        <path d="M40 110 A80 80 0 0 1 200 110"
              stroke="url(#widthMask)rawliteral" +
             prefix + R"rawliteral(")" stroke-width="18" fill="none" />
      </mask>
    </defs>

    <path d="M40 110 A80 80 0 0 1 200 110" fill="none" stroke="#ddd" stroke-width="20"/>
    <path d="M40 110 A80 80 0 0 1 200 110"
          fill="none"
          stroke="url(#colorGradient)rawliteral" +
             prefix + R"rawliteral("
          stroke-width="18"
          stroke-linecap="round"
          mask="url(#fadeMask)rawliteral" +
             prefix + R"rawliteral(")"/>

    <g id="marks)rawliteral" +
             prefix + R"rawliteral(" stroke="#444" stroke-width="1"></g>
    <g id="labels)rawliteral" +
             prefix + R"rawliteral(" font-size="10" fill="#222" text-anchor="middle" font-family="sans-serif"></g>

    <line id="needle)rawliteral" +
             prefix + R"rawliteral(" x1="120" y1="110" x2="120" y2="50" stroke="#000" stroke-width="3" stroke-linecap="round" />
    <circle cx="120" cy="110" r="5" fill="#222"/>
  </svg>

  <div id="meterLabel)rawliteral" +
             prefix + R"rawliteral(" style="text-align:center; font-size:1.3em; margin-top:6px; font-weight:bold;">--</div>
  </div>
</div>
)rawliteral";

  content += R"rawliteral(
<script>
  (function() {
    const svgNS = "http://www.w3.org/2000/svg";
    const centerX = 120, centerY = 110, radius = 80;
    const tickStart = radius + 2, tickEnd = radius + 14, labelRadius = radius + 24;
    const minVal = )rawliteral" +
             String(minValue, 3) + R"rawliteral(;
    const maxVal = )rawliteral" +
             String(maxValue, 3) + R"rawliteral(;
    const minAngle = -90, maxAngle = 90;

    const prefix = ")rawliteral" +
             prefix + R"rawliteral(";

    function valueToAngle(val) {
      val = Math.max(minVal, Math.min(val, maxVal));
      let ratio = (val - minVal) / (maxVal - minVal);
      return minAngle + ratio * (maxAngle - minAngle);
    }

    function polarToCartesian(cx, cy, r, angleDegrees) {
      let angleRad = (angleDegrees - 90) * Math.PI / 180.0;
      return { x: cx + r * Math.cos(angleRad), y: cy + r * Math.sin(angleRad) };
    }

    const ticks = [];
    for (let v = minVal; v <= maxVal; v += (maxVal - minVal) / 6) ticks.push(v);

    const marksGroup = document.getElementById("marks" + prefix);
    const labelsGroup = document.getElementById("labels" + prefix);
    ticks.forEach(v => {
      let angle = valueToAngle(v);
      let start = polarToCartesian(centerX, centerY, tickStart, angle);
      let end = polarToCartesian(centerX, centerY, tickEnd, angle);
      let line = document.createElementNS(svgNS, "line");
      line.setAttribute("x1", start.x); line.setAttribute("y1", start.y);
      line.setAttribute("x2", end.x);   line.setAttribute("y2", end.y);
      line.setAttribute("stroke", "#444"); line.setAttribute("stroke-width", "1.5");
      marksGroup.appendChild(line);

      let labelPos = polarToCartesian(centerX, centerY, labelRadius, angle);
      let text = document.createElementNS(svgNS, "text");
      text.setAttribute("x", labelPos.x); text.setAttribute("y", labelPos.y + 3); // iets optillen
      text.textContent = v.toFixed(1);
      labelsGroup.appendChild(text);
    });

    function updateMeter(val) {
      const angle = valueToAngle(val);
      const needle = document.getElementById("needle" + prefix);
      needle.setAttribute("transform", "rotate(" + angle + " " + centerX + " " + centerY + ")");
      document.getElementById("meterLabel" + prefix).textContent = val.toFixed(2);
    }

    async function fetchData() {
      try {
        const res = await fetch('/data.json');
        if (!res.ok) throw new Error("Geen geldige respons");
        const data = await res.json();
        const channel = data.channels.find(c => c.name === ")rawliteral" +
             channel + R"rawliteral(");
        if (channel && channel.hasOwnProperty(")rawliteral" +
             field + R"rawliteral(")) {
          updateMeter(channel[")rawliteral" +
             field + R"rawliteral("]);
        }
      } catch (err) {
        console.error("Fout bij data ophalen:", err);
      }
    }

    setInterval(fetchData, 1000);
    fetchData();
  })();
</script>
)rawliteral";

  return content;
}

//  --  generateTotalsBlock  --  //
static String generateTotalsBlock(const String &channel)
{
  String prefix = channel;
  prefix.replace(" ", "_");

  String content;

  content += "<div class=\"totals-container\" style=\"text-align:center; font-size:1.1em; color:#333; margin-top:10px;\">\n";
  content += "  <div>Total Ah: <span id=\"totalAh_" + prefix + "\">--</span></div>\n";
  content += "  <div>Total Wh: <span id=\"totalWh_" + prefix + "\">--</span></div>\n";
  content += "  <div>Battery Capacity Ah: <span id=\"capacity_" + prefix + "\">--</span></div>\n";
  content += "</div>\n";

  content += "<script>\n";
  content += "(function() {\n";
  content += "  async function fetchTotals_" + prefix + "() {\n";
  content += "    try {\n";
  content += "      const res = await fetch('/data.json');\n";
  content += "      if (!res.ok) throw new Error('Geen geldige respons');\n";
  content += "      const data = await res.json();\n";
  content += "      const channel = data.channels.find(c => c.name === '" + channel + "');\n";
  content += "      if (channel) {\n";
  content += "        if (typeof channel.totalAh === 'number')\n";
  content += "          document.getElementById('totalAh_" + prefix + "').textContent = channel.totalAh.toFixed(2);\n";
  content += "        if (typeof channel.totalWh === 'number')\n";
  content += "          document.getElementById('totalWh_" + prefix + "').textContent = channel.totalWh.toFixed(2);\n";
  content += "        if (typeof channel.batteryCapacityAh === 'number')\n";
  content += "          document.getElementById('capacity_" + prefix + "').textContent = channel.batteryCapacityAh.toFixed(2);\n";
  content += "      }\n";
  content += "    } catch (err) {\n";
  content += "      console.error('Fout bij totaaldata ophalen:', err);\n";
  content += "    }\n";
  content += "  }\n";
  content += "  setInterval(fetchTotals_" + prefix + ", 1000);\n";
  content += "  fetchTotals_" + prefix + "();\n";
  content += "})();\n";
  content += "</script>\n";

  return content;
}

//  --  handleMeters  --  //
void RouteMeters::handleMeters(AsyncWebServerRequest *request)
{
  std::vector<std::pair<int, String>> kleurzonesVolt;
  kleurzonesVolt.push_back({0, "#ff0000"});   // Rood
  kleurzonesVolt.push_back({10, "#ff6600"});  // Oranje-rood
  kleurzonesVolt.push_back({20, "#ffaa00"});  // Oranje
  kleurzonesVolt.push_back({30, "#ffee00"});  // Geel
  kleurzonesVolt.push_back({40, "#ccff00"});  // Geelgroen
  kleurzonesVolt.push_back({50, "#99ff00"});  // Lichtgroen
  kleurzonesVolt.push_back({60, "#66ff00"});  // Groen
  kleurzonesVolt.push_back({70, "#33cc33"});  // Donkergroen
  kleurzonesVolt.push_back({80, "#00cc66"});  // Turquoise
  kleurzonesVolt.push_back({90, "#009999"});  // Blauwgroen
  kleurzonesVolt.push_back({100, "#0066cc"}); // Blauw

  std::vector<std::pair<int, String>> kleurzonesAmpere = {
      {0, "#66ff00"},  // -100% = groen
      {50, "#00cc66"}, // 0% = Turquoise
      {100, "#ffee00"} // +100% = Geel
  };
  struct KanaalConfig
  {
    String naam;
    bool showTotals;
    bool actief; // bepaalt of het kanaal standaard getoond wordt
    float minVolt, maxVolt;
    float minAmp, maxAmp;
    float minWatt, maxWatt;
  };

  std::vector<KanaalConfig> kanalen = {
      {"MPPT", true, true, 10.0, 15.0, -5.0, 5.0, -50.0, 100.0},
      {"Accu 1", true, true, 10.0, 15.0, -10.0, 10.0, -150.0, 150.0},
      {"Accu 2", true, true, 10.0, 15.0, -10.0, 10.0, -150.0, 150.0}};

  String content;

  // CSS voor toggle switches
  content += R"rawliteral(
<style>
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
  margin-left: 10px;
  margin-right: 20px;
  vertical-align: middle;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0; left: 0; right: 0; bottom: 0;
  background-color: #ccc;
  transition: .4s;
  border-radius: 34px;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: .4s;
  border-radius: 50%;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:checked + .slider:before {
  transform: translateX(26px);
}
</style>
)rawliteral";

  // Toggle switches bovenaan ipv sliders
  content += R"rawliteral(<div style="text-align:center; margin-bottom: 20px;">)rawliteral";

  // content += "<h3>Toon kanalen</h3>\n";

  content += "<fieldset class=\"info-box\">\n";
  content += "  <h1>" + langManager.get("meters.title") + "</h1>\n";
  content += "</fieldset>\n";

  for (const KanaalConfig &kanaal : kanalen)
  {
    String prefix = kanaal.naam;
    prefix.replace(" ", "");

    content += "<label for=\"switch" + prefix + "\">" + kanaal.naam + "</label> ";

    content += "<label class=\"switch\">";
    content += "<input type=\"checkbox\" id=\"switch" + prefix + "\"";
    if (kanaal.actief)
      content += " checked";
    content += ">";
    content += "<span class=\"slider\"></span>";
    content += "</label>";
  }
  content += "</div>\n";

  // JavaScript voor toggle switches (checkboxen)
  content += R"rawliteral(
<script>
  let channelStatus = {};

  function updateChannelVisibility() {
    for (const [key, val] of Object.entries(channelStatus)) {
      const el = document.getElementById('channel_' + key);
      if (el) el.style.display = val == 1 ? 'block' : 'none';
    }
  }

  function saveSettings() {
    localStorage.setItem('channelStatus', JSON.stringify(channelStatus));
  }

  function loadSettings() {
    let saved = localStorage.getItem('channelStatus');
    if (saved) {
      channelStatus = JSON.parse(saved);
      for (const [key, val] of Object.entries(channelStatus)) {
        let checkbox = document.getElementById('switch' + key);
        if (checkbox) checkbox.checked = val == 1;
      }
      updateChannelVisibility();
    } else {
      document.querySelectorAll('input[type=checkbox][id^=switch]').forEach(el => {
        let key = el.id.replace('switch', '');
        channelStatus[key] = el.checked ? 1 : 0;
      });
      updateChannelVisibility();
    }
  }

  document.addEventListener('DOMContentLoaded', () => {
    document.querySelectorAll('input[type=checkbox][id^=switch]').forEach(el => {
      el.addEventListener('change', e => {
        let key = e.target.id.replace('switch', '');
        channelStatus[key] = e.target.checked ? 1 : 0;
        updateChannelVisibility();
        saveSettings();
      });
    });
    loadSettings();
  });
</script>
)rawliteral";

  // Container met kanalen horizontaal uitgelijnd
  content += R"rawliteral(
<div style="display:flex; justify-content:center; gap:20px; flex-wrap:wrap; margin-top: 20px;">
)rawliteral";

  for (const KanaalConfig &kanaal : kanalen)
  {
    String prefix = kanaal.naam;
    prefix.replace(" ", "");

    content += "<div id=\"channel_" + prefix + "\" style=\"border:1px solid #aaa; border-radius:8px; padding:15px; max-width:280px; background:#fafafa; box-shadow: 0 2px 6px rgba(0,0,0,0.1);\">\n";
    content += "<h2 style=\"text-align:center; margin-bottom:15px; font-family: sans-serif;\">" + kanaal.naam + "</h2>\n";

    content += generateSingleMeterSvg(kanaal.naam, "voltage", kanaal.minVolt, kanaal.maxVolt, "Spanning (V)", kleurzonesVolt);
    content += generateSingleMeterSvg(kanaal.naam, "current", kanaal.minAmp, kanaal.maxAmp, "Stroom (A)", kleurzonesAmpere);
    content += generateSingleMeterSvg(kanaal.naam, "power", kanaal.minWatt, kanaal.maxWatt, "Vermogen (W)", kleurzonesAmpere);

    if (kanaal.showTotals)
    {
      content += generateTotalsBlock(kanaal.naam);
    }

    content += "</div>\n";
  }

  content += "</div>\n";

  // Verstuur response
  request->send(200, "text/html", HtmlUtils::renderPage(content, "Meters"));
}

//  --  registerRoutes  --  //
void RouteMeters::registerRoutes(AsyncWebServer &server)
{
  server.on("/meters", HTTP_GET, handleMeters);
}
