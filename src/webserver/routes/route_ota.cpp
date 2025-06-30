#include "route_ota.h"
#include "route_reboot.h"
#include "../route_base.h"

bool rebootRequested = false;

// -- generateOtaUploadPage -- //
String generateOtaUploadPage()
{
  String extraCss = R"rawliteral(
<style>
  .upload-form {
      display: flex;
      flex-direction: column;
      gap: 15px;
      margin-top: 20px;
      max-width: 400px;
  }
  .custom-file-input {
      display: flex;
      flex-direction: column;
      gap: 5px;
  }
  .custom-file-input label {
      display: inline-block;
      background-color: #3498db;
      color: white;
      padding: 8px 12px;
      border-radius: 5px;
      cursor: pointer;
      width: fit-content;
  }
  .custom-file-input label:hover {
      background-color: #2980b9;
  }
  #filenameDisplay {
      font-style: italic;
      font-size: 0.9em;
  }
  #progressBar {
      width: 100%;
      background-color: #ddd;
      margin-top: 10px;
      height: 20px;
  }
  #progress {
      width: 0%;
      height: 100%;
      background-color: #4CAF50;
      text-align: center;
      color: white;
  }
</style>
)rawliteral";

  String content = "<fieldset class=\"info-box\">\n"
                   "<h1>" +
                   langManager.get("ota.upload.title") + "</h1>\n"
                                                         "</fieldset>\n"
                                                         "<fieldset class=\"info-box\">\n"
                                                         "<form class=\"upload-form\" id=\"otaForm\" method=\"POST\" action=\"/ota_upload\" enctype=\"multipart/form-data\">\n"

                                                         "<div class=\"custom-file-input\">\n"
                                                         "<label for=\"firmwareInput\" id=\"firmwareLabel\">📁 " +
                   langManager.get("ota.upload.choose") + "</label>\n"
                                                          "<input type=\"file\" id=\"firmwareInput\" name=\"firmware\" required hidden />\n"
                                                          "<span id=\"filenameDisplay\">" +
                   langManager.get("ota.upload.nofile") + "</span>\n"
                                                          "</div>\n"

                                                          "<button type=\"submit\">" +
                   langManager.get("ota.upload.button") + "</button>\n"
                                                          "</form>\n"
                                                          "<div id=\"status\"></div>\n"
                                                          "<div id=\"progressBar\" aria-label=\"Upload voortgang\">\n"
                                                          "<div id=\"progress\"></div>\n"
                                                          "</div>\n"
                                                          "</fieldset>\n";

  String extraJs = "<script>\n"
                   "const uploadingText = `" +
                   langManager.get("ota.upload.progress") + "`;\n"
                                                            "const errorText = `" +
                   langManager.get("ota.upload.error") + "`;\n"
                                                         "const errorStatusText = `" +
                   langManager.get("ota.upload.error.status") + "`;\n"
                                                                "const noFileText = `" +
                   langManager.get("ota.upload.nofile") + "`;\n"

                                                          "let uploadInProgress = false;\n"

                                                          "const firmwareInput = document.getElementById('firmwareInput');\n"
                                                          "const firmwareLabel = document.getElementById('firmwareLabel');\n"
                                                          "const filenameDisplay = document.getElementById('filenameDisplay');\n"

                                                          "firmwareInput.addEventListener('change', (e) => {\n"
                                                          "  const file = e.target.files[0];\n"
                                                          "  filenameDisplay.textContent = file ? file.name : noFileText;\n"
                                                          "});\n"

                                                          "document.getElementById('otaForm').addEventListener('submit', function(e) {\n"
                                                          "  e.preventDefault();\n"
                                                          "  if (uploadInProgress) return;\n"
                                                          "  uploadInProgress = true;\n"

                                                          "  const file = firmwareInput.files[0];\n"
                                                          "  const progressBar = document.getElementById('progress');\n"
                                                          "  const status = document.getElementById('status');\n"

                                                          "  if (!file) {\n"
                                                          "    status.textContent = noFileText;\n"
                                                          "    uploadInProgress = false;\n"
                                                          "    return;\n"
                                                          "  }\n"

                                                          "  const formData = new FormData();\n"
                                                          "  formData.append('firmware', file);\n"

                                                          "  const xhr = new XMLHttpRequest();\n"
                                                          "  xhr.open('POST', '/ota_upload', true);\n"

                                                          "  xhr.upload.onprogress = function(event) {\n"
                                                          "    if (event.lengthComputable) {\n"
                                                          "      let percent = (event.loaded / event.total) * 100;\n"
                                                          "      progressBar.style.width = percent + '%';\n"
                                                          "      status.textContent = uploadingText.replace('{percent}', percent.toFixed(0));\n"
                                                          "    }\n"
                                                          "  };\n"

                                                          "  xhr.onload = function() {\n"
                                                          "    if (xhr.status === 200) {\n"
                                                          "      window.location.href = '/ota_reboot';\n"
                                                          "    } else {\n"
                                                          "      status.textContent = errorStatusText.replace('{status}', xhr.statusText);\n"
                                                          "      progressBar.style.width = '0%';\n"
                                                          "      uploadInProgress = false;\n"
                                                          "    }\n"
                                                          "  };\n"

                                                          "  xhr.onerror = function() {\n"
                                                          "    status.textContent = errorText;\n"
                                                          "    progressBar.style.width = '0%';\n"
                                                          "    uploadInProgress = false;\n"
                                                          "  };\n"

                                                          "  xhr.send(formData);\n"
                                                          "});\n"
                                                          "</script>\n";

  return HtmlUtils::renderPage(content, langManager.get("ota.upload.title"), "/ota", extraCss, extraJs);
}

// -- generateOtaRebootPage -- //
String generateOtaRebootPage()
{
  String content = "<fieldset class=\"info-box\">\n"
                   "<h2>" +
                   langManager.get("ota.upload.success.title") + "</h2>\n"
                                                                 "<p>" +
                   langManager.get("ota.upload.success.text") + "</p>\n"
                                                                "<div class=\"spinner\"></div>\n"
                                                                "<p>" +
                   langManager.get("ota.upload.wait") + "</p>\n"
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
fetch("/ota_reboot_ping", { method: "POST" });

function checkAlive() {
    fetch("/", { cache: "no-store" }).then(res => {
        if (res.ok) window.location.href = "/";
    }).catch(() => setTimeout(checkAlive, 1000));
}

setTimeout(checkAlive, 3000);
</script>
)rawliteral";

  return HtmlUtils::renderPage(content, langManager.get("ota.upload.success.title"), "/ota_reboot", css, js);
}

// -- namespace RouteOTA -- //
namespace RouteOTA
{
  void handleOtaUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
  {
    if (index == 0)
    {
      DEBUG_LOG("[OTA] Start update");
      if (!Update.begin())
      {
        DEBUG_LOG("[OTA] Begin mislukt");
      }
    }

    if (len)
    {
      if (Update.write(data, len) != len)
      {
        DEBUG_LOG("[OTA] Write mislukt");
      }
    }

    if (final)
    {
      if (Update.end(true))
      {
        DEBUG_LOG("[OTA] Update succesvol. Grootte: " + String(index + len));
      }
      else
      {
        DEBUG_LOG("[OTA] Einde update mislukt");
      }
    }
  }

  void registerRoutes(AsyncWebServer &server)
  {
    server.on("/ota", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", generateOtaUploadPage()); });

    server.on("/ota_upload", HTTP_POST, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "OK"); }, handleOtaUpload);

    server.on("/ota_reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", generateOtaRebootPage()); });

    server.on("/ota_reboot_ping", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                DEBUG_LOGF("[OTA] Ping ontvangen. Reboot starten in 2 sec...");
                request->send(200, "text/plain", "OK");
                RouteReboot::scheduleReboot(2); });
  }
}
