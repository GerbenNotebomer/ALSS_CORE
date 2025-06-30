#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <map>    // nodig voor std::map
#include <vector> // nodig voor std::vector

struct NavItem
{
    String path;
    String emoji;
    std::map<String, String> labels; // bv. {"nl": "Home", "en": "Home"}
};

extern std::vector<NavItem> navItems;

struct NavItemInfo
{
    String key;  // sleutel zoals "nav.home"
    String path; // route zoals "/"
};

extern const std::vector<NavItemInfo> navItemInfo;
class HtmlUtils
{
public:
    static String escapeHtml(const String &input);
    static String baseTemplate();
    static String renderPage(const String &contentHtml, const String &title, const String &activePath = "/", const String &extraCss = "", const String &extraJs = "");
    static String generateNavBar(const String &currentPath, const String &lang = "nl");

    static String sharedCss();
    static String navBarCss();
    static String generateFooter();

    static String generateProgressBar(const String &id = "progressBar", const String &label = "Voortgang", int maxValue = 100);
    // Overloaden modal pagina functies
    static String generateModalPage(const String &title, const String &message);
    static String generateModalPage(const String &title, const String &message, const String &redirectUrl);
    static String generateModalRedirectPage(const String &title = "Melding", const String &message = "Onbekende melding", const String &redirectUrl = "/", int seconds = 5);
    static String logoBase64();
    static String getFlagEmoji(const String &lang);
};
