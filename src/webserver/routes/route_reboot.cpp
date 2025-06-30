#include "route_reboot.h"
#include "route_ota.h"
#include "../../common/common_includes.h"

Ticker RouteReboot::rebootTimer;

//  --  scheduleReboot  --  //
void RouteReboot::scheduleReboot(float delaySeconds)
{
    DEBUG_LOGF("[scheduleReboot] Reboot gepland over %.1f seconden\n", delaySeconds);
    rebootTimer.once(delaySeconds, []()
                     {
        DEBUG_LOG("[scheduleReboot] Flush logbuffer vóór reboot...");
        SDCardUtils::flushLogBuffer();   // Flush logs veilig wegschrijven
        delay(100);                      // korte pauze om schrijven zeker te laten afronden
        DEBUG_LOG("[scheduleReboot] ALSS WebServer wordt nu herstart...\n");
        ESP.restart(); });
}

//  --  registerRoutes  --  //
void RouteReboot::registerRoutes(AsyncWebServer &server)
{
    server.on("/reboot/confirm", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        DEBUG_LOG("[registerRoutes] Bevestiging ontvangen, ALSS WebServer zal herstarten\n");


        String html = 
    "<fieldset class=\"info-box\">\n"
    "  <h2>🚀 " + langManager.get("reboot.herstart.title") + "</h2>\n"
    "  <p>" + langManager.get("reboot.herstart.message1") + "</p>\n"
    "  <div class=\"spinner\"></div>\n"
    "  <p>" + langManager.get("reboot.herstart.message2") + "</p>\n"
    "</fieldset>\n";

        String css = R"rawliteral(
            <style>
            .spinner {
                margin: 20px auto;
                border: 8px solid #f3f3f3;
                border-top: 8px solid #3498db;
                border-radius: 50%;
                width: 60px;
                height: 60px;
                animation: spin 1s linear infinite;
            }
            @keyframes spin {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }
            </style>
        )rawliteral";

        String js = R"rawliteral(
            <script>
            fetch("/reboot_ping", { method: "POST" });

            function checkIfBackOnline() {
                fetch("/", { cache: "no-store" }).then(res => {
                    if (res.ok) window.location.href = "/";
                }).catch(() => setTimeout(checkIfBackOnline, 1000));
            }

            setTimeout(checkIfBackOnline, 3000);
            </script>
        )rawliteral";

        request->send(200, "text/html", HtmlUtils::renderPage(html, langManager.get("reboot.herstart.title"), "/", css, js)); });

    server.on("/reboot_ping", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        DEBUG_LOG("[registerRoutes] Ping ontvangen, ALLS WebServer herstart over 2 sec...");
        request->send(200, "text/plain", "OK");
        RouteReboot::scheduleReboot(2); });

    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    DEBUG_LOG("[registerRoutes] Herstart bevestiging gevraagd\n");
    
    String html;
    html += "<h2>🔁 " + langManager.get("reboot.bevestiging.title") + "</h2>";
    html += "<p>" + langManager.get("reboot.bevestiging.text") + "</p>";
    html += "<div class='button-container' style='margin-top: 20px;'>";
    html += "<a href='/reboot/confirm' class='btn-confirm-yes'>" + langManager.get("button.ja") + "</a>&nbsp;&nbsp;";
    html += "<a href='/' class='btn-confirm-no'>" + langManager.get("button.annuleren") + "</a>";
    html += "</div>";
    
    request->send(200, "text/html", HtmlUtils::renderPage(
        html,
        langManager.get("reboot.bevestiging.title"),
        "/", "", ""
    )); });
}
