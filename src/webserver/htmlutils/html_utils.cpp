#include "html_utils.h"
#include "../../common/common_includes.h"

const std::vector<NavItemInfo> navItemInfo = {
    {"nav.home", "/"},
    {"nav.dashboard", "/dashboard"},
    {"nav.meters", "/meters"},
    {"nav.calibratie", "/calibratie"},
    {"nav.debuglog", "/debuglog/view"},
    {"nav.instellingen", "/instellingen"},
    {"nav.ota", "/ota"},
    {"nav.reboot", "/reboot"},
    {"nav.sd", "/sd"},
    {"nav.weersvoorspelling", "/weersvoorspelling"},
};

//  --  baseTemplate  --  //
String HtmlUtils::baseTemplate()
{
    return R"rawliteral(
<!DOCTYPE html>
<html lang="nl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>[[TITLE]]</title>
    [[CSS]]
    [[EXTRA_CSS]]
</head>
<body>
    [[NAVBAR]]
    <main class="container">
        [[CONTENT]]
    </main>
    [[FOOTER]]
    [[EXTRA_JS]]
</body>
</html>
)rawliteral";
}

//  --  renderPage  --  //
String HtmlUtils::renderPage(const String &contentHtml, const String &title, const String &activePath, const String &extraCss, const String &extraJs)
{
    String html = baseTemplate();
    html.replace("[[TITLE]]", title);
    html.replace("[[CSS]]", sharedCss());
    html.replace("[[NAVBAR]]", generateNavBar(activePath));
    html.replace("[[FOOTER]]", generateFooter());
    html.replace("[[CONTENT]]", contentHtml);
    html.replace("[[EXTRA_CSS]]", extraCss);
    html.replace("[[EXTRA_JS]]", extraJs);
    return html;
}

//  --  generateNavBar  --  //
String HtmlUtils::generateNavBar(const String &currentPath, const String &lang)
{
    String pathOnly = currentPath;
    int qIndex = pathOnly.indexOf('?');
    if (qIndex != -1)
        pathOnly = pathOnly.substring(0, qIndex);

    String html = navBarCss();

    html += R"rawliteral(
<nav class="main-nav">
    <div class="nav-wrapper">
<div class="nav-left">
    <div class="nav-logo"></div>
    <button class="hamburger" onclick="toggleMenu()" aria-label="Menu" aria-expanded="false">
        <div></div><div></div><div></div>
    </button>
</div>

        <div class="nav-links" id="navLinks">
)rawliteral";

    for (const auto &item : navItemInfo)
    {
        String label = langManager.get(item.key);
        String emoji, text;

        int spaceIndex = label.indexOf(' ');
        if (spaceIndex != -1)
        {
            emoji = label.substring(0, spaceIndex);
            text = label.substring(spaceIndex + 1);
        }
        else
        {
            emoji = "";
            text = label;
        }

        html += "<a href=\"" + item.path + "\" class=\"nav-link\" data-path=\"" + item.path + "\" data-emoji=\"" + emoji + "\">" + emoji + " " + text + "</a>\n";
    }

    html += R"rawliteral(
        </div>
    </div>
</nav>

<script>
function toggleMenu() {
    const nav = document.getElementById("navLinks");
    const btn = document.querySelector(".hamburger");
    const expanded = btn.getAttribute("aria-expanded") === "true";
    nav.classList.toggle("active");
    btn.classList.toggle("active");
    btn.setAttribute("aria-expanded", !expanded);
}

document.addEventListener('DOMContentLoaded', () => {
    const currentPath = window.location.pathname;
    const links = document.querySelectorAll('.nav-link');
    links.forEach(link => {
        const linkPath = link.dataset.path;
        if (linkPath === "/") {
            if (currentPath === "/") link.classList.add("active");
        } else {
            if (currentPath === linkPath || currentPath.startsWith(linkPath + "/"))
                link.classList.add("active");
        }
    });
});
</script>
)rawliteral";

    return html;
}

// -- navBarCss --
String HtmlUtils::navBarCss()
{
    File cssFile = LittleFS.open("/navbar.css", "r");
    if (!cssFile)
    {
        DEBUG_LOG("⚠️ Kan /navbar.css niet openen!");
        return "<!-- Kan navBar CSS niet laden -->";
    }

    String css = "<style>\n";
    while (cssFile.available())
    {
        css += cssFile.readStringUntil('\n') + "\n";
    }
    css += "</style>";
    cssFile.close();
    return css;
}

//  --  generateFooter  --  //
String HtmlUtils::generateFooter()
{
    String year = __DATE__;
    year = year.substring(year.lastIndexOf(' ') + 1); // haalt "2025" uit "Jun 08 2025"
    return "<footer style=\"position: relative; bottom: 0; width: 100%; text-align: center; font-size: 0.75em; color: #999; padding: 10px 12px; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\">\n"
           "&copy; " +
           year + " G.K. Notebomer – ALSS Webserver\n"
                  "</footer>";
}

//  --  sharedCss  --  //
String HtmlUtils::sharedCss()
{
    File cssFile = LittleFS.open("/shared.css", "r");
    if (!cssFile)
    {
        DEBUG_LOG("⚠️ Kan /shared.css niet openen!");
        return "<!-- Kan CSS niet laden -->";
    }

    String css = "<style>\n";
    while (cssFile.available())
    {
        css += cssFile.readStringUntil('\n') + "\n";
    }
    css += "</style>";
    cssFile.close();
    return css;
}

//  --  generateProgressBar  --  //
String HtmlUtils::generateProgressBar(const String &id, const String &label, int maxValue)
{
    String html;
    html += "<label for=\"" + id + "\">" + label + ":</label><br>";
    html += "<progress id=\"" + id + "\" max=\"" + String(maxValue) + "\" value=\"0\" style=\"width: 100%; height: 25px;\"></progress>";
    html += R"rawliteral(
<script>
function updateProgressBar(id, value) {
    const bar = document.getElementById(id);
    if(bar) {
        bar.value = value;
        if(value >= bar.max) {
            console.log('Upload/actie voltooid!');
            // Hier kun je eventueel extra actie uitvoeren
        }
    }
}
</script>
)rawliteral";
    return html;
}

//  --  generateModalPage  --  //
String HtmlUtils::generateModalPage(const String &title, const String &message)
{
    return generateModalPage(title, message, "/sd");
}

String HtmlUtils::generateModalPage(const String &title, const String &message, const String &redirectUrl)
{
    String buttonLabel = langManager.get("button.terug");

    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>)rawliteral" +
           title + R"rawliteral(</title>
  )rawliteral" +
           HtmlUtils::sharedCss() + R"rawliteral(
</head>
<body>
  <div class="modal" style="display:flex; align-items:center; justify-content:center; height:100vh;">
    <div class="modal-content">
      <span class="modal-close" onclick="window.location.href=')rawliteral" +
           redirectUrl + R"rawliteral(';">&times;</span>
      <h2>)rawliteral" +
           title + R"rawliteral(</h2>
      <p>)rawliteral" +
           message + R"rawliteral(</p>
      <div class="modal-button-group">
        <button onclick="window.location.href=')rawliteral" +
           redirectUrl + R"rawliteral(';">)rawliteral" + buttonLabel + R"rawliteral(</button>
      </div>
    </div>
  </div>
</body>
</html>
)rawliteral";
}

//  --  generateModalRedirectPage  --  //
String HtmlUtils::generateModalRedirectPage(const String &title, const String &message, const String &redirectUrl, int seconds)
{
    String html;
    html += "<!DOCTYPE html><html lang='nl'><head><meta charset='UTF-8'>";
    html += "<title>" + title + "</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<meta http-equiv='refresh' content='" + String(seconds) + "; URL=" + redirectUrl + "' />";
    html += "<style>"
            "body { margin:0; font-family:sans-serif; background-color:rgba(0,0,0,0.1); }"
            ".modal { display:flex; justify-content:center; align-items:center; height:100vh; background-color:rgba(0,0,0,0.5); }"
            ".modal-content { background:white; padding:30px; border-radius:10px; text-align:center; max-width:400px; width:90%; box-shadow:0 0 10px rgba(0,0,0,0.3); }"
            ".spinner { margin:20px auto; width:40px; height:40px; border:4px solid #ccc; border-top:4px solid #333; border-radius:50%; animation:spin 1s linear infinite; }"
            "@keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }"
            ".countdown { font-size:1.1em; margin-top:10px; color:#555; }"
            "</style>";
    html += "</head><body>";
    html += "<div class='modal'><div class='modal-content'>";
    html += "<h2>" + title + "</h2>";
    html += "<p>" + message + "</p>";
    html += "<div class='spinner'></div>";
    html += "<p class='countdown'>Je wordt over <span id='count'>" + String(seconds) + "</span> seconde(n) doorgestuurd...</p>";
    html += "</div></div>";
    html += "<script>"
            "let count = " +
            String(seconds) + ";"
                              "let el = document.getElementById('count');"
                              "let timer = setInterval(() => {"
                              "  count--; el.textContent = count;"
                              "  if (count <= 0) { clearInterval(timer); window.location.href='" +
            redirectUrl + "'; }"
                          "}, 1000);"
                          "</script>";
    html += "</body></html>";

    return html;
}

//  --  escapeHtml  --  //
String HtmlUtils::escapeHtml(const String &input)
{
    String output;
    output.reserve(input.length());

    for (size_t i = 0; i < input.length(); i++)
    {
        char c = input.charAt(i);
        switch (c)
        {
        case '&':
            output += "&amp;";
            break;
        case '<':
            output += "&lt;";
            break;
        case '>':
            output += "&gt;";
            break;
        case '"':
            output += "&quot;";
            break;
        case '\'':
            output += "&#39;";
            break;
        default:
            output += c;
        }
    }

    return output;
}
