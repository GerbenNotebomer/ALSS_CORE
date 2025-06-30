#include "route_sd_card.h"
#include "../route_base.h"

// --  ensureSdCardAvailable  --  //
static bool ensureSdCardAvailable()
{
  if (SD.begin())
  {
    DEBUG_LOGF("[SD] Herinitialisatie succesvol.\n");
    return true;
  }
  else
  {
    DEBUG_LOGF("[SD] Herinitialisatie mislukt.\n");
    return false;
  }
}

//  --  urlEncode  --  //
static String urlEncode(const String &str)
{
  String encoded = "";
  for (int i = 0; i < str.length(); i++)
  {
    char c = str.charAt(i);
    if (('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z') ||
        ('0' <= c && c <= '9') ||
        c == '-' || c == '_' || c == '.' || c == '~' || c == '/')
    {
      encoded += c;
    }
    else
    {
      encoded += '%';
      uint8_t b = (uint8_t)c;
      char hex1 = "0123456789ABCDEF"[b >> 4];
      char hex2 = "0123456789ABCDEF"[b & 0x0F];
      encoded += hex1;
      encoded += hex2;
    }
  }
  return encoded;
}

//  --  urlDecode  --  //
static String urlDecode(const String &str)
{
  String decoded = "";
  for (int i = 0; i < str.length(); i++)
  {
    char c = str.charAt(i);
    if (c == '%' && i + 2 < str.length())
    {
      char hex[3] = {str.charAt(i + 1), str.charAt(i + 2), 0};
      int val = strtol(hex, nullptr, 16);
      decoded += (char)val;
      i += 2;
    }
    else if (c == '+')
    {
      decoded += ' '; // plus wordt spatie (standaard in URL encoding)
    }
    else
    {
      decoded += c;
    }
  }
  return decoded;
}

//  --  urlEncodeRedirectParam  --  //
static String urlEncodeRedirectParam(const String &str)
{
  String enc = urlEncode(str);
  // Zorg dat spaties in redirects altijd %20 zijn, geen '+'
  enc.replace("+", "%20");
  return enc;
}

//  --  registerRoutes  --  //
void RouteSdCard::registerRoutes(AsyncWebServer &server)
{
  // Helper: valideer en decodeer param, prepend slash als nodig
  auto getParamPath = [](AsyncWebServerRequest *request, const char *param, bool post = false) -> String
  {
    if (!request->hasParam(param, post))
      return "";
    String val = urlDecode(request->getParam(param, post)->value());
    if (!val.startsWith("/"))
      val = "/" + val;
    return val;
  };

  // Helper: stuur 404 fout bij niet bestaand bestand/map
  auto sendNotFound = [](AsyncWebServerRequest *request, const String &path, const char *type = "Bestand")
  {
    DEBUG_LOGF("[registerRoutes] %s niet gevonden: %s\n", type, path.c_str());
    request->send(404, "text/html", HtmlUtils::generateModalPage("Foutmelding", String(type) + " niet gevonden: " + path));
  };

  // ---- Download route ----
  server.on("/sd/download", HTTP_GET, [getParamPath, sendNotFound](AsyncWebServerRequest *request)
            {
        String filePath = getParamPath(request, "file");
        if (filePath == "") {
            DEBUG_LOGF("[registerRoutes] Bestand niet gespecificeerd (download)\n");
            request->send(400, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Bestand niet gespecificeerd"));
            return;
        }
        DEBUG_LOGF("[registerRoutes] Download request: %s\n", filePath.c_str());
        if (!SD.exists(filePath.c_str())) {
            sendNotFound(request, filePath);
            return;
        }
        AsyncWebServerResponse *response = request->beginResponse(SD, filePath.c_str(), "application/octet-stream");
        String filename = filePath.substring(filePath.lastIndexOf('/') + 1);
        response->addHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");
        request->send(response); });

  // ---- Delete route ----
  server.on("/sd/delete", HTTP_GET, [getParamPath, sendNotFound](AsyncWebServerRequest *request)
            {
        String filePath = getParamPath(request, "file");
        if (filePath == "") {
            DEBUG_LOGF("[registerRoutes] Bestand niet gespecificeerd (delete)\n");
            request->send(400, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Bestand niet gespecificeerd"));
            return;
        }
        DEBUG_LOGF("[registerRoutes] Delete request: %s\n", filePath.c_str());
        if (!SD.exists(filePath.c_str())) {
            sendNotFound(request, filePath);
            return;
        }
        if (SD.remove(filePath.c_str())) {
            int lastSlash = filePath.lastIndexOf('/');
            String parent = (lastSlash > 0) ? filePath.substring(0, lastSlash) : "/";
            DEBUG_LOGF("[registerRoutes] Bestand verwijderd, redirect naar: %s\n", parent.c_str());
            request->redirect("/sd?path=" + urlEncodeRedirectParam(parent));
        } else {
            DEBUG_LOGF("[registerRoutes] Verwijderen mislukt: %s\n", filePath.c_str());
            request->send(500, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Verwijderen mislukt: " + filePath));
        } });

  // ---- Upload route ----
  server.on(
      "/sd/upload",
      HTTP_POST,
      [](AsyncWebServerRequest *request)
      {
        String redirectPath = "/";
        if (request->hasParam("path", true))
        {
          redirectPath = urlDecode(request->getParam("path", true)->value());
          if (!redirectPath.startsWith("/"))
            redirectPath = "/" + redirectPath;
        }
        DEBUG_LOGF("[registerRoutes] Upload redirect naar: %s\n", redirectPath.c_str());
        request->redirect("/sd?path=" + urlEncodeRedirectParam(redirectPath));
      },
      handleUploadBody,
      nullptr);

  // ---- View route (tekstbestanden bekijken) ----
  server.on("/sd/view", HTTP_GET, [getParamPath, sendNotFound](AsyncWebServerRequest *request)
            {
        String filePath = getParamPath(request, "file");
        if (filePath == "") {
            DEBUG_LOGF("[registerRoutes] Bestand niet gespecificeerd (view)\n");
            request->send(400, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Bestand niet gespecificeerd"));
            return;
        }
        DEBUG_LOGF("[registerRoutes] View request: %s\n", filePath.c_str());
        if (!SD.exists(filePath.c_str())) {
            sendNotFound(request, filePath);
            return;
        }
        File file = SD.open(filePath, FILE_READ);
        if (!file) {
            request->send(500, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Kan bestand niet openen: " + filePath));
            return;
        }
        String ext = filePath.substring(filePath.lastIndexOf('.') + 1);
        ext.toLowerCase();
        if (ext == "txt" || ext == "log" || ext == "csv" || ext == "json" || ext == "xml") {
            String rawContent;
            while (file.available()) rawContent += (char)file.read();
            file.close();
            String contentHtml;
            contentHtml += "<h2>📄 Bekijken van: <code>" + filePath + "</code></h2>";
            contentHtml += "<div class='editor-container'>";
            contentHtml += "<textarea rows='25' style='width:100%; font-family: monospace;' readonly>" + HtmlUtils::escapeHtml(rawContent) + "</textarea>";
            contentHtml += "</div>";
            contentHtml += "<div class='button-container' style='margin-top: 15px;'>";
            contentHtml += "<a href='/sd?path=" + urlEncode(filePath.substring(0, filePath.lastIndexOf('/'))) + "' class='btn-confirm-no'>Sluiten</a>";
            contentHtml += "</div>";
            request->send(200, "text/html", HtmlUtils::renderPage(contentHtml, "Bestand bekijken", "/sd", "", ""));
            return;
        }
              // Afbeeldingen direct serveren
      if (ext == "bmp" || ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif") {
          String contentType;
          if (ext == "bmp") contentType = "image/bmp";
          else if (ext == "jpg" || ext == "jpeg") contentType = "image/jpeg";
          else if (ext == "png") contentType = "image/png";
          else if (ext == "gif") contentType = "image/gif";
          else contentType = "application/octet-stream";

          AsyncWebServerResponse *response = request->beginResponse(SD, filePath.c_str(), contentType);
          request->send(response);
          file.close();
          return;
      }
        file.close();
        request->send(415, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Bestandstype niet ondersteund: " + ext)); });

  // ---- Maak nieuwe map ----
  server.on("/sd/mkdir", HTTP_POST, [getParamPath](AsyncWebServerRequest *request)
            {
        if (!request->hasParam("path", true) || !request->hasParam("dirname", true)) {
            request->send(400, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Ongeldige parameters voor aanmaken map"));
            return;
        }
        String path = getParamPath(request, "path", true);
        String dirname = urlDecode(request->getParam("dirname", true)->value());
        if (!dirname.startsWith("/")) dirname = "/" + dirname;
        String fullPath = path + dirname;
        DEBUG_LOGF("[registerRoutes] Nieuwe map maken: %s\n", fullPath.c_str());
        if (SD.mkdir(fullPath.c_str())) {
            request->redirect("/sd?path=" + urlEncodeRedirectParam(path));
        } else {
            request->send(500, "text/html", HtmlUtils::generateModalPage("Foutmelding", "Maken van map mislukt: " + fullPath));
        } });

  // ---- Verwijder lege map ----
  server.on("/sd/rmdir", HTTP_GET, [getParamPath, sendNotFound](AsyncWebServerRequest *request)
            {
    String dirPath = getParamPath(request, "dir");
    if (dirPath == "") {
        String title = langManager.get("algemeen.fout");
        String msg = langManager.get("sdkaart.mapnietgespecificeerd");
        request->send(400, "text/html", HtmlUtils::generateModalPage(title.c_str(), msg.c_str()));
        return;
    }
     DEBUG_LOGF("[registerRoutes] Map verwijderen: %s\n", dirPath.c_str());

    File dir = SD.open(dirPath.c_str());
    if (!dir || !dir.isDirectory()) {
        sendNotFound(request, dirPath, langManager.get("sdkaart.kanmapnietopenen").c_str());
        return;
    }

    // Controleer leeg
    File entry = dir.openNextFile();
    if (entry) {
        entry.close();
        dir.close();
        String title = langManager.get("algemeen.fout");
        // Hier gebruik je de label voor "Map is niet leeg"
        String msg = langManager.get("sdkaart.mapnietleeg") + ": " + dirPath;
        request->send(400, "text/html", HtmlUtils::generateModalPage(title.c_str(), msg.c_str()));
        return;
    }

    dir.close();

    if (SD.rmdir(dirPath.c_str())) {
        int lastSlash = dirPath.lastIndexOf('/');
        String parent = (lastSlash > 0) ? dirPath.substring(0, lastSlash) : "/";
        request->redirect("/sd?path=" + urlEncodeRedirectParam(parent));
    } else {
        String title = langManager.get("algemeen.fout");
        String msg = langManager.get("sdkaart.verwijderen.mislukt") + ": " + dirPath;
        request->send(500, "text/html", HtmlUtils::generateModalPage(title.c_str(), msg.c_str()));
    } });
  // ---- Root route (altijd laatste) ----
  server.on("/sd", HTTP_GET, handleRoot);
}

//  --  handleRoot  --  //
void RouteSdCard::handleRoot(AsyncWebServerRequest *request)
{
  // Basispad instellen op root
  String path = "/";
  if (request->hasParam("path"))
  {
    path = urlDecode(request->getParam("path")->value());

    // Zorg dat pad altijd met een '/' begint
    if (!path.startsWith("/"))
    {
      path = "/" + path;
    }

    // Verwijder trailing slashes, behalve bij root
    while (path.length() > 1 && path.endsWith("/"))
    {
      path.remove(path.length() - 1);
    }
  }

  DEBUG_LOGF("[handleRoot] Gevraagd pad: %s\n", path.c_str());

  // Probeer de map te openen
  File dir = SD.open(path.c_str());
  if (!dir)
  {
    DEBUG_LOGF("[handleRoot] ❌ Pad bestaat niet of SD niet beschikbaar: %s\n", path.c_str());
    String html = HtmlUtils::generateModalRedirectPage("Fout", "Kan het pad niet openen: " + path, "/", 5);
    request->send(404, "text/html", html);
    return;
  }

  if (!dir.isDirectory())
  {
    DEBUG_LOGF("[handleRoot] ❌ Geen map: %s\n", path.c_str());
    String html = HtmlUtils::generateModalRedirectPage("Geen map", path + " is geen map.", "/", 5);
    request->send(400, "text/html", html);
    dir.close();
    return;
  }

  DEBUG_LOGF("[handleRoot] ✅ Map geopend: %s\n", path.c_str());

  // Genereer HTML en stuur het terug
  String html = generateHtml(path);
  request->send(200, "text/html", html);
  dir.close();
}

//  --  handleUploadBody  --  //
void RouteSdCard::handleUploadBody(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
  static File uploadFile;
  static String uploadPath;

  if (index == 0)
  {
    if (request->hasParam("path", true))
    {
      uploadPath = urlDecode(request->getParam("path", true)->value());
      if (!uploadPath.startsWith("/"))
        uploadPath = "/" + uploadPath;
      while (uploadPath.length() > 1 && uploadPath.endsWith("/"))
      {
        uploadPath = uploadPath.substring(0, uploadPath.length() - 1);
      }
    }
    else
    {
      uploadPath = "/";
    }

    String fullFilePath = uploadPath + "/" + filename;
    DEBUG_LOGF("[handleUploadBody] Upload start: %s\n", fullFilePath.c_str());

    if (uploadFile)
      uploadFile.close();
    uploadFile = SD.open(fullFilePath.c_str(), FILE_WRITE);

    if (!uploadFile)
    {
      DEBUG_LOGF("[handleUploadBody] Kan bestand niet openen voor schrijven: %s\n", fullFilePath.c_str());
      return;
    }
  }

  if (len)
  {
    uploadFile.write(data, len);
  }

  if (final)
  {
    uploadFile.close();
    DEBUG_LOGF("[handleUploadBody] Upload compleet: %s\n", filename.c_str());
  }
}

//  --  generateHtml  --  //
String RouteSdCard::generateHtml(const String &path)
{
  String content = generateHtmlContent(path);
  String title = "SD-kaart Inhoud: " + path;

  String extraCss = R"css(
    #progressBar {
      width: 100%;
      background-color: #eee;
      border-radius: 6px;
      margin-top: 15px;
      height: 20px;
      overflow: hidden;
    }
    #progress {
      height: 100%;
      width: 0%;
      background-color: #3498db;
      transition: width 0.3s ease;
    }
    #status {
      margin-top: 10px;
      font-weight: bold;
    }
  )css";

  return HtmlUtils::renderPage(content, title, path, "<style>" + extraCss + "</style>", "");
}

//  --  generateHtmlContent  --  //
String RouteSdCard::generateHtmlContent(const String &path)
{
  DEBUG_LOGF("[generateHtmlContent] Genereren lijst voor map: %s\n", path.c_str());

  String html = "<fieldset class='info-box'><h1>📁 " + langManager.get("sdkaart.inhoudvan") + ": " + path + "</h1>";

  if (path != "/")
  {
    int lastSlash = path.lastIndexOf('/');
    String parentPath = (lastSlash > 0) ? path.substring(0, lastSlash) : "/";
    html += "<a class='nav-up' href='/sd?path=" + urlEncode(parentPath) + "'>⬆️ " + langManager.get("button.terug") + "</a>";
  }

  html += "</fieldset>";

  html += "<fieldset class='info-box'><legend>📄 " + langManager.get("sdkaart.inhoudvan") + "</legend><section class='file-list'>";

  File dir = SD.open(path.c_str());
  if (!dir || !dir.isDirectory())
  {
    html += "<div class='msg'>❌ " + langManager.get("sdkaart.kanmapnietopenen") + ": " + path + "</div>";
    DEBUG_LOGF("[generateHtmlContent] Kan map niet openen (in generateHtmlContent): %s\n", path.c_str());
    return html;
  }

  File entry = dir.openNextFile();
  while (entry)
  {
    String name = String(entry.name());

    if (name == "System Volume Information" || name.startsWith("."))
    {
      entry.close();
      entry = dir.openNextFile();
      continue;
    }

    String displayName = name;
    String fullPath = (path == "/") ? "/" + name : path + "/" + name;
    String encodedPath = urlEncode(fullPath);

    html += "<div class='file-item'>";

    if (entry.isDirectory())
    {
      html += "📁 <a href='/sd?path=" + encodedPath + "'><strong>" + displayName + "/</strong></a>";
      html += "<div class='file-actions'>";
      html += "<a href='/sd/rmdir?dir=" + encodedPath + "' title='" + langManager.get("button.ja") + "'><strong>[🗑️]</strong></a>";
      html += "</div>";
    }
    else
    {
      html += "📄 <span>" + displayName + "</span>";
      html += "<div class='file-actions'>";
      html += "<a href='/sd/view?file=" + encodedPath + "' title='Bekijk'><strong>[👁️]</strong></a>";
      html += "<a href='/sd/download?file=" + encodedPath + "' title='Download'><strong>[⬇️]</strong></a>";
      html += "<button class='btn-delete' data-file='" + encodedPath + "' title='" + langManager.get("sdkaart.verwijderbevestiging") + "'>[🗑️]</button>";
      html += "</div>";
    }

    html += "</div>";
    entry.close();
    entry = dir.openNextFile();
  }
  dir.close();

  html += "</section></fieldset>";

  // Uploadformulier
  html += R"rawliteral(
    <hr>
    <fieldset class="info-box">
      <legend><h2>)rawliteral" +
          langManager.get("sdkaart.uploadennaar") + ": " + path + R"rawliteral(</h2></legend>
      <form id="uploadForm" method="POST" action="/sd/upload" enctype="multipart/form-data">
        <input type="hidden" name="path" value=")rawliteral" +
          path + R"rawliteral(">
        <input type="file" name="file" required>
        <input type="submit" value=")rawliteral" +
          langManager.get("sdkaart.upload") + R"rawliteral(">
      </form>
      <div id="progressBar"><div id="progress"></div></div>
      <div id="status"></div>
    </fieldset>
  )rawliteral";

  // Nieuwe map aanmaken
  html += R"rawliteral(
    <hr>
    <fieldset class="info-box">
      <legend><h2>📂 )rawliteral" +
          langManager.get("sdkaart.maakmapaan") + ": " + path + R"rawliteral(</h2></legend>
      <form method="POST" action="/sd/mkdir">
        <input type="hidden" name="path" value=")rawliteral" +
          path + R"rawliteral(">
        <input type="text" name="dirname" placeholder="Naam van nieuwe map" required>
        <input type="submit" value=")rawliteral" +
          langManager.get("sdkaart.maakmapaan") + R"rawliteral(">
      </form>
    </fieldset>
  )rawliteral";

  // Modals
  html += R"rawliteral(
  <div id="uploadModal" class="modal">
    <div class="modal-content">
      <span class="modal-close" id="uploadModalClose">&times;</span>
      <h3>)rawliteral" +
          langManager.get("sdkaart.upload.voltooid") + R"rawliteral(</h3>
      <button class="btn-confirm-yes" id="uploadModalOk">)rawliteral" +
          langManager.get("button.sluiten") + R"rawliteral(</button>
    </div>
  </div>

  <div id="deleteModal" class="modal">
    <div class="modal-content">
      <span class="modal-close" id="deleteModalClose">&times;</span>
      <h3>)rawliteral" +
          langManager.get("sdkaart.verwijderbevestiging") + R"rawliteral(</h3>
      <button class="btn-confirm-yes" id="deleteYes">)rawliteral" +
          langManager.get("button.ja") + R"rawliteral(</button>
      <button class="btn-confirm-no" id="deleteNo">)rawliteral" +
          langManager.get("button.nee") + R"rawliteral(</button>
    </div>
  </div>
)rawliteral";

  // JavaScript met vertaalde strings
  html += R"rawliteral(
  <script>
    const form = document.getElementById('uploadForm');
    const progressBar = document.getElementById('progress');
    const status = document.getElementById('status');
    const uploadModal = document.getElementById('uploadModal');
    const uploadModalClose = document.getElementById('uploadModalClose');
    const uploadModalOk = document.getElementById('uploadModalOk');

    const uploadCompleteText = ")rawliteral" +
          langManager.get("sdkaart.upload.voltooid") + R"rawliteral(";
    const uploadErrorText = ")rawliteral" +
          langManager.get("sdkaart.upload.fout") + R"rawliteral(";
    const uploadStatusText = ")rawliteral" +
          langManager.get("sdkaart.upload.status") + R"rawliteral(";
    const deleteFailedText = ")rawliteral" +
          langManager.get("sdkaart.verwijderen.mislukt") + R"rawliteral(";
    const deleteErrorText = ")rawliteral" +
          langManager.get("sdkaart.verwijderen.fout") + R"rawliteral(";

    form.addEventListener('submit', function(event) {
      event.preventDefault();
      progressBar.style.width = '0%';
      status.textContent = '';
      const xhr = new XMLHttpRequest();
      const formData = new FormData(form);
      xhr.upload.addEventListener('progress', function(e) {
        if (e.lengthComputable) {
          const percent = Math.round((e.loaded / e.total) * 100);
          progressBar.style.width = percent + '%';
          status.textContent = uploadStatusText.replace('{percent}', percent + '%');
        }
      });
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4) {
          if (xhr.status == 200) {
            progressBar.style.width = '100%';
            status.textContent = uploadCompleteText;
            uploadModal.style.display = 'flex';
          } else {
            status.textContent = uploadErrorText + xhr.status;
          }
        }
      };
      xhr.open('POST', form.action, true);
      xhr.send(formData);
    });

    function closeUploadModal() {
      uploadModal.style.display = 'none';
      location.reload();
    }

    uploadModalClose.onclick = closeUploadModal;
    uploadModalOk.onclick = closeUploadModal;

    const deleteModal = document.getElementById('deleteModal');
    const deleteModalClose = document.getElementById('deleteModalClose');
    const deleteYesBtn = document.getElementById('deleteYes');
    const deleteNoBtn = document.getElementById('deleteNo');
    let fileToDelete = null;

    document.querySelectorAll('.btn-delete').forEach(button => {
      button.addEventListener('click', () => {
        fileToDelete = button.getAttribute('data-file');
        deleteModal.style.display = 'flex';
      });
    });

    function closeDeleteModal() {
      deleteModal.style.display = 'none';
      fileToDelete = null;
    }

    deleteModalClose.onclick = closeDeleteModal;
    deleteNoBtn.onclick = closeDeleteModal;

    deleteYesBtn.onclick = function() {
      if (!fileToDelete) return;
      fetch('/sd/delete?file=' + encodeURIComponent(fileToDelete), { method: 'GET' })
        .then(response => {
          if (response.ok) {
            closeDeleteModal();
            location.reload();
          } else {
            alert(deleteFailedText + response.status);
            closeDeleteModal();
          }
        })
        .catch(err => {
          alert(deleteErrorText + err);
          closeDeleteModal();
        });
    };

    window.onclick = function(event) {
      if (event.target == uploadModal) closeUploadModal();
      if (event.target == deleteModal) closeDeleteModal();
    };

    document.querySelectorAll('nav a').forEach(link => {
      const linkHref = link.getAttribute('href');
      if (window.location.pathname === linkHref || window.location.href.startsWith(linkHref + '?')) {
        link.classList.add('active');
      }
    });
  </script>
)rawliteral";

  return html;
}
