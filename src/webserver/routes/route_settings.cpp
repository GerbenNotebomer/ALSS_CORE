#include "route_settings.h"
#include "../../common/common_includes.h"

const char *SETTINGS_FILE = "/config/parameters.txt";

// -- Helper: key=value pairs uitlezen --
std::map<String, String> parseSettings(const String &content)
{
    std::map<String, String> settings;
    int start = 0;
    while (start < content.length())
    {
        int end = content.indexOf('\n', start);
        if (end == -1)
            end = content.length();
        String line = content.substring(start, end);
        int sep = line.indexOf('=');
        if (sep > 0)
        {
            String key = line.substring(0, sep);
            String val = line.substring(sep + 1);
            key.trim();
            val.trim();
            settings[key] = val;
        }
        start = end + 1;
    }
    return settings;
}

// -- Helper: map terug omzetten naar bestandinhoud --
String serializeSettings(const std::map<String, String> &settings)
{
    String output;
    for (const auto &kv : settings)
    {
        output += kv.first + "=" + kv.second + "\n";
    }
    return output;
}

// -- Bestand lezen en parsen --
std::map<String, String> readSettings()
{
    if (!SD.exists(SETTINGS_FILE))
        return {};

    File file = SD.open(SETTINGS_FILE, FILE_READ);
    if (!file)
        return {};

    String content;
    while (file.available())
        content += (char)file.read();
    file.close();

    return parseSettings(content);
}

// -- Bestand schrijven --
bool writeSettings(const std::map<String, String> &settings)
{
    File file = SD.open(SETTINGS_FILE, FILE_WRITE);
    if (!file)
        return false;

    String serialized = serializeSettings(settings);
    file.print(serialized);
    file.close();
    return true;
}

//// ---- HTML formulier genereren ---- ////

//  --  beautifyLabel  --  //
String beautifyLabel(const String &raw, const String &prefix)
{
    String label = raw;

    // Prefix verwijderen
    if (prefix.length() > 0 && label.startsWith(prefix + "_"))
        label = label.substring(prefix.length() + 1);

    // Underscores naar spaties
    label.replace('_', ' ');

    // Eerste letter hoofdletter
    if (label.length() > 0)
        label.setCharAt(0, toupper(label.charAt(0)));

    return label;
}

//  --  generateForm  --  //
String generateForm(const std::map<String, String> &settings)
{
    std::map<String, std::vector<std::pair<String, String>>> groupedSettings;

    // Instellingen groeperen per prefix
    for (const auto &kv : settings)
    {
        int sep = kv.first.indexOf('_');
        String group = (sep != -1) ? kv.first.substring(0, sep) : "overig";
        groupedSettings[group].push_back(kv);
    }

    String html;
    html += "<fieldset class=\"info-box\">\n";
    html += "  <h1>" + langManager.get("instellingen.title") + "</h1>\n";
    html += "</fieldset>\n";

    html += "<form method='POST' action='/instellingen/save'>\n";

    // === Taalkeuze ===
    html += "<fieldset class='info-box'>\n";
    html += "<legend>" + langManager.get("instellingen.taal.legend") + "</legend>\n";
    html += "<div class='field-row'>\n";
    html += "<label for='language'>" + langManager.get("instellingen.taal.label") + "</label>\n";
    html += "<select name='language' id='language'>\n";

    std::vector<String> availableLangs = langManager.getAvailableLanguages();
    for (const auto &langCode : availableLangs)
    {
        String label = langManager.get("taal." + langCode);
        if (label.startsWith("{"))
            label = langCode;

        bool selected = (langCode == langManager.getCurrentLanguage());
        html += "<option value='" + langCode + "'" + (selected ? " selected" : "") + ">" + label + "</option>\n";
    }

    html += "</select>\n";
    html += "</div>\n";
    html += "</fieldset>\n";

    // Eerst alle andere groepen dan 'Timers'
    for (const auto &group : groupedSettings)
    {
        if (group.first == "Timers")
            continue;

        String label = group.first;
        if (label.length() > 0)
        {
            char firstChar = toupper(label.charAt(0));
            label = String(firstChar) + label.substring(1);
        }

        html += "<fieldset class='info-box'>\n";
        html += "<legend>📁 " + label + "</legend>\n";

        for (const auto &kv : group.second)
        {
            html += "<div class='field-row'>\n";

            String displayLabel = beautifyLabel(kv.first, group.first);
            html += "  <label for='" + kv.first + "'>" + displayLabel + "</label>\n";
            html += "  <input type='text' name='" + kv.first + "' id='" + kv.first + "' value='" + HtmlUtils::escapeHtml(kv.second) + "' required>\n";
            html += "</div>\n";
        }

        html += "</fieldset>\n";
    }

    // Nu pas de groep 'Timers'
    auto timersIt = groupedSettings.find("Timers");
    if (timersIt != groupedSettings.end())
    {
        String label = "Timers";
        char firstChar = toupper(label.charAt(0));
        label = String(firstChar) + label.substring(1);

        html += "<fieldset class='info-box'>\n";
        html += "<legend>⏱️ " + label + "</legend>\n";

        for (const auto &kv : timersIt->second)
        {
            html += "<div class='field-row'>\n";

            String displayLabel = beautifyLabel(kv.first, "Timers");
            html += "  <label for='" + kv.first + "'>" + displayLabel + "</label>\n";

            long ms = kv.second.toInt();
            int h = ms / 3600000;
            int m = (ms % 3600000) / 60000;
            int s = (ms % 60000) / 1000;

            html += "  <input type='number' name='" + kv.first + "_h' min='0' style='width: 60px;' value='" + String(h) + "'> h ";
            html += "  <input type='number' name='" + kv.first + "_m' min='0' max='59' style='width: 60px;' value='" + String(m) + "'> m ";
            html += "  <input type='number' name='" + kv.first + "_s' min='0' max='59' style='width: 60px;' value='" + String(s) + "'> s\n";

            html += "</div>\n";
        }

        html += "</fieldset>\n";
    }

    html += "<div class='button-container'>\n";
    html += "<button type='submit' class='btn-confirm-yes'>" + langManager.get("button.opslaan") + "</button>\n";
    html += "<a href='/' class='btn-confirm-no'>" + langManager.get("button.annuleren") + "</a>\n";
    html += "</div>\n";

    html += "</form>\n";
    return html;
}

// -- Routes registreren --
void RouteSettings::registerRoutes(AsyncWebServer &server)
{
    // === GET: Instellingenpagina tonen ===
    server.on("/instellingen", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        auto settings = readSettings();
        String pageContent = generateForm(settings);
        String fullPage = HtmlUtils::renderPage(pageContent, "Instellingen", "/instellingen", "", "");
        request->send(200, "text/html", fullPage); });

    // === POST: Instellingen opslaan ===
    server.on("/instellingen/save", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    auto currentSettings = readSettings();

    for (auto &kv : currentSettings)
    {
        if (kv.first.startsWith("Timers_"))
        {
            String hParam = kv.first + "_h";
            String mParam = kv.first + "_m";
            String sParam = kv.first + "_s";

            int h = request->hasParam(hParam.c_str(), true) ? request->getParam(hParam.c_str(), true)->value().toInt() : 0;
            int m = request->hasParam(mParam.c_str(), true) ? request->getParam(mParam.c_str(), true)->value().toInt() : 0;
            int s = request->hasParam(sParam.c_str(), true) ? request->getParam(sParam.c_str(), true)->value().toInt() : 0;

            long ms = ((long)h * 3600 + m * 60 + s) * 1000;
            kv.second = String(ms);
        }
        else if (request->hasParam(kv.first.c_str(), true))
        {
            kv.second = request->getParam(kv.first.c_str(), true)->value();
        }
    }

    // Taal opslaan
    bool languageSaved = false;
    if (request->hasParam("language", true))
    {
        String langCode = request->getParam("language", true)->value();
        EEPROM_Manager::saveLanguageToEEPROM(langCode);
        currentLanguage = langCode;
        langManager.setLanguage(currentLanguage);
        languageSaved = true;
    }

    bool settingsSaved = writeSettings(currentSettings);

    if (settingsSaved)
    {
        // ** Hier direct RAM-parameters bijwerken **
        updateParametersFromMap(currentSettings);
    }

    if (settingsSaved || languageSaved)
    {
        request->send(200, "text/html", HtmlUtils::generateModalPage(
            "✅ " + langManager.get("algemeen.succes"),
            langManager.get("instellingen.opgeslagen"),
            "/instellingen"));
    }
    else
    {
        request->send(500, "text/html", HtmlUtils::generateModalPage(
            "❌ " + langManager.get("algemeen.fout"),
            langManager.get("instellingen.opslaanmislukt"),
            "/instellingen"));
    } });
}
