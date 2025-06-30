#pragma once

#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <vector>

class LanguageManager
{
public:
    LanguageManager(const String &langCode = "");
    ~LanguageManager();

    void setLanguage(const String &lang);
    String getCurrentLanguage() const;

    bool loadTranslationsFromJson(const String &path = "/lang/translations.json");
    bool init(const String &defaultLangPath = "/lang/translations.json");
    String get(const String &key) const;
    std::vector<String> getAvailableLanguages() const;

private:
    DynamicJsonDocument *langDoc = nullptr;

    // ✅ HIER toevoegen!
    String currentLang;

    static constexpr const char *defaultLang = "nl";
    static constexpr const char *fallbackLang = "en";
};

extern LanguageManager langManager;
