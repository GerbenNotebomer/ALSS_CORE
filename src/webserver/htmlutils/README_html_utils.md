# HtmlUtils Module

Deze module bevat hulpmethoden voor het genereren van HTML-pagina’s en webinterface-elementen voor een embedded webserver (ESP32/ESP8266).

---

## Functionaliteit

- Bouwen van een basis HTML-template met placeholders voor titel, CSS, navigatiebalk, content en footer.
- Genereren van een dynamische navigatiebalk met ondersteuning voor meertalige labels en emoji’s.
- Inlezen van CSS-bestanden uit LittleFS (`/shared.css` en `/navbar.css`) en inline injectie in de pagina.
- Genereren van footer met jaartal en auteursvermelding.
- Genereren van progress bars met JavaScript updatefunctie.
- Genereren van modale pagina’s (popups) voor meldingen en redirects, met automatische terug- of doorstuurfunctionaliteit.
- Veilig HTML-escaping van invoerstrings ter voorkoming van XSS.

---

## Gebruik

### Basis HTML-pagina genereren

```cpp
String html = HtmlUtils::renderPage(
    "<h1>Welkom</h1><p>Dit is de content.</p>",  // contentHtml
    "Homepagina",                                  // titel
    "/dashboard",                                  // actieve pad in navbar
    "",                                            // extra CSS
    ""                                             // extra JavaScript
);

Navigatiebalk genereren (optioneel los)

String navbar = HtmlUtils::generateNavBar("/meters", "nl");
CSS bestanden in LittleFS plaatsen
/shared.css voor algemene styles

/navbar.css voor navigatiebalk styles

Modale pagina's

String modalHtml = HtmlUtils::generateModalPage("Foutmelding", "Er is iets misgegaan.", "/dashboard");
Of met automatische redirect na enkele seconden:


String redirectHtml = HtmlUtils::generateModalRedirectPage(
    "Bezig met laden...",
    "Je wordt doorgestuurd naar het dashboard.",
    "/dashboard",
    3
);
Veilig HTML-escaping
Gebruik escapeHtml() om gebruikersinvoer of variabele tekst veilig in HTML te plaatsen:


String safeString = HtmlUtils::escapeHtml(userInput);
Structuur
navItemInfo bevat navigatie-items met sleutels en paden.

Labels en emoji’s worden dynamisch geladen via langManager.get().

Navigatiebalk gebruikt een hamburger-menu voor mobiel.

Alle CSS wordt inline geladen uit LittleFS.

JavaScript regelt het togglen van het menu en markeert de actieve link.

Afhankelijkheden
Arduino.h

LittleFS.h

STL vector & map (voor navigatie-items)

#include "html_utils.h"

void setup() {
    LittleFS.begin();
    Serial.begin(115200);

    String content = "<h2>Welkom op de dashboard</h2>";
    String page = HtmlUtils::renderPage(content, "Dashboard", "/dashboard");
    Serial.println(page);
}
Auteur
G.K. Notebomer – ALSS Webserver
2025, MIT License