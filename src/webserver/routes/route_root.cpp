#include "route_root.h"
#include "../../common/common_includes.h"

//  --  generateRootPage  --  //
String generateRootPage(const String &boardName, const String &systemInfo)
{
    String extraCss = R"rawliteral(
<style>
    .qr-section {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
        gap: 20px;
        margin-top: 20px;
    }
    .qr-box {
        flex: 1;
        min-width: 260px;
        text-align: center;
        background-color: #fff;
        padding: 15px;
        border-radius: 10px;
        box-shadow: 0 0 8px rgba(0,0,0,0.1);
    }
    .qr-code {
        margin: 10px auto;
        width: 200px;
        height: 200px;
    }
    .system-info {
        margin-top: 10px;
        font-family: monospace;
        background-color: #f9f9f9;
        padding: 8px 12px;
        border-radius: 6px;
        font-size: 0.95em;
    }
    .page-title {
        display: flex;
        align-items: center;
        gap: 15px;
        padding-bottom: 10px;
    }
    .logo {
        height: 50px;
        vertical-align: middle;
    }
</style>
)rawliteral";

    // Dynamisch LAN IP ophalen
    String localIpStr = WiFi.localIP().toString();
    String localNetworkUrl = "http://" + localIpStr + "/";
    const String apUrl = "http://192.168.4.1/";

    String content = R"rawliteral(
<fieldset class="info-box">
  <div class="page-title">
    <img src="/logo/ALSS_Icoon_108x72.jpg" alt="Logo" class="logo">
    <h1>)rawliteral" +
                     langManager.get("root.title") + R"rawliteral(</h1>
  </div>
</fieldset>

<fieldset class="info-box">
    <legend>📋 )rawliteral" +
                     langManager.get("root.systeminfo") + R"rawliteral(</legend>
    <p><strong>)rawliteral" +
                     langManager.get("root.activeboard") + R"rawliteral(:</strong> )rawliteral" + boardName + R"rawliteral(</p>
    <div id="system-info" class="system-info">)rawliteral" +
                     systemInfo + R"rawliteral(</div>
</fieldset>

<fieldset class="info-box">
    <legend>🔌 )rawliteral" +
                     langManager.get("root.connect_dashboard") + R"rawliteral(</legend>
    <div class="qr-section">
        <div class="qr-box">
            <h3>1. )rawliteral" +
                     langManager.get("root.connect_ap") + R"rawliteral(</h3>
            <div id="wifi_qr" class="qr-code"></div>
            <p><code>)rawliteral" +
                     langManager.get("root.ssid") + R"rawliteral(: ALLS_AP<br>)rawliteral" + langManager.get("root.password") + R"rawliteral(: 123456789</code></p>
            <p>)rawliteral" +
                     langManager.get("root.open") + R"rawliteral(: <a href=")rawliteral" + apUrl + R"rawliteral(">)rawliteral" + apUrl + R"rawliteral(</a></p>
            <div id="url_ap_qr" class="qr-code"></div>
        </div>
        <div class="qr-box">
            <h3>2. )rawliteral" +
                     langManager.get("root.connect_wifi") + R"rawliteral(</h3>
            <p><strong>)rawliteral" +
                     langManager.get("root.open_dashboard_wifi") + R"rawliteral(</strong></p>
            <p><a href=")rawliteral" +
                     localNetworkUrl + R"rawliteral(">)rawliteral" + localNetworkUrl + R"rawliteral(</a></p>
            <div id="url_lan_qr" class="qr-code"></div>
        </div>
    </div>
</fieldset>
)rawliteral";

    String extraJs = R"rawliteral(
<script src="https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
<script>
    window.addEventListener("DOMContentLoaded", () => {
        new QRCode(document.getElementById("wifi_qr"), {
            text: "WIFI:S:ALLS_AP;T:WPA;P:123456789;",
            width: 200,
            height: 200
        });

        new QRCode(document.getElementById("url_ap_qr"), {
            text: "http://192.168.4.1/",
            width: 200,
            height: 200
        });

        new QRCode(document.getElementById("url_lan_qr"), {
            text: ")rawliteral" +
                     localNetworkUrl + R"rawliteral(",
            width: 200,
            height: 200
        });
    });

    setInterval(() => {
        fetch('/systeminfo')
            .then(response => response.text())
            .then(data => {
                document.getElementById('system-info').innerHTML = data;
            });
    }, 1000);
</script>
)rawliteral";

    return HtmlUtils::renderPage(content, langManager.get("root.title"), "/", extraCss, extraJs);
}

//  --  handleRoot  --  //
void RouteRoot::handleRoot(AsyncWebServerRequest *request)
{
    String boardName = getBoardName();
    String systemInfo = getSystemInfo();
    request->send(200, "text/html", generateRootPage(boardName, systemInfo));
}

//  --  registerRoutes  --  //
void RouteRoot::registerRoutes(AsyncWebServer &server)
{
    server.on("/", HTTP_GET, handleRoot);

    // ✅ Live systeeminfo
    server.on("/systeminfo", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String info = getSystemInfo();
        request->send(200, "text/plain", info); });

    // ✅ 404 - Niet gevonden pagina met redirect en spinner
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        String url = request->url();
        DEBUG_LOGF("[RouteRoot] Niet gevonden: %s\n", url.c_str());

        String html = HtmlUtils::renderPage(
            R"rawliteral(
            <style>
                .centered {
                    text-align: center;
                    margin-top: 60px;
                }
                .spinner {
                    border: 8px solid #f3f3f3;
                    border-top: 8px solid #3498db;
                    border-radius: 50%;
                    width: 60px;
                    height: 60px;
                    animation: spin 1s linear infinite;
                    margin: 0 auto 20px auto;
                }
                @keyframes spin {
                    0% { transform: rotate(0deg); }
                    100% { transform: rotate(360deg); }
                }
                .countdown {
                    font-size: 1.2em;
                    font-weight: bold;
                    color: #555;
                }
            </style>
            <div class="centered">
                <div class="spinner"></div>
                <h3>)rawliteral" + langManager.get("root.page_not_found") + R"rawliteral( <code>)rawliteral" + url + R"rawliteral(</code></h3>
                <p>)rawliteral" + langManager.get("root.redirecting_home") + R"rawliteral( <span class="countdown" id="countdown">5</span> )rawliteral" + langManager.get("root.seconds") + R"rawliteral(...)</p>
            </div>
            <script>
                let seconds = 5;
                const countdown = document.getElementById('countdown');
                const interval = setInterval(() => {
                    seconds--;
                    countdown.textContent = seconds;
                    if (seconds <= 0) {
                        clearInterval(interval);
                        window.location.href = "/";
                    }
                }, 1000);
            </script>
            )rawliteral",
            langManager.get("root.page_not_found_title")
        );

        request->send(404, "text/html", html); });
}
