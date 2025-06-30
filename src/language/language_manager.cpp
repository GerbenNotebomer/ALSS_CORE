#include "language_manager.h"
#include "../common/common_includes.h"

LanguageManager langManager;

//  --  LanguageManager  --  //
LanguageManager::LanguageManager(const String &langCode)
{
    String lang = langCode;
    if (lang.isEmpty())
        lang = defaultLang; // fallback op "nl"
    setLanguage(lang);
}

// Destructor: netjes geheugen vrijmaken
//  --  LanguageManager  --  //
LanguageManager::~LanguageManager()
{
    if (langDoc)
    {
        delete langDoc;
        langDoc = nullptr;
    }
}

//  --  setLanguage  --  //
void LanguageManager::setLanguage(const String &lang)
{
    currentLang = lang;
    currentLang.trim();
    currentLang.toLowerCase();
    DEBUG_LOGF("🌐 Taal ingesteld op: %s", currentLang.c_str());
}

//  --  loadTranslationsFromJson  --  //
bool LanguageManager::loadTranslationsFromJson(const String &path)
{
    if (langDoc)
    {
        delete langDoc;
        langDoc = nullptr;
    }

    File file = LittleFS.open(path, "r");
    if (!file)
    {
        DEBUG_LOGF("❌ Kan taalbestand niet openen: %s", path.c_str());
        return false;
    }

    langDoc = new DynamicJsonDocument(65536); // 64KB PSRAM/heap

    DeserializationError err = deserializeJson(*langDoc, file);
    file.close();

    if (err)
    {
        DEBUG_LOGF("❌ JSON parse-fout in %s: %s", path.c_str(), err.c_str());
        delete langDoc;
        langDoc = nullptr;
        return false;
    }

    DEBUG_LOG("✅ Vertalingen succesvol geladen uit JSON");
    return true;
}

//  --  get  --  //
String LanguageManager::get(const String &key) const
{
    if (!langDoc || !langDoc->containsKey(key))
        return "{" + key + "}";

    JsonVariantConst entry = (*langDoc)[key];

    if (!entry.is<JsonObjectConst>())
        return "{" + key + "}";

    JsonObjectConst translations = entry.as<JsonObjectConst>();

    if (translations.containsKey(currentLang))
        return translations[currentLang].as<const char *>();

    if (translations.containsKey(fallbackLang))
        return translations[fallbackLang].as<const char *>();

    return "{" + key + "}";
}

//  --  getAvailableLanguages  --  //
std::vector<String> LanguageManager::getAvailableLanguages() const
{
    std::vector<String> langs;

    if (!langDoc)
        return langs;

    for (JsonPairConst kv : langDoc->as<JsonObjectConst>())
    {
        JsonObjectConst translations = kv.value().as<JsonObjectConst>();
        for (JsonPairConst item : translations)
        {
            langs.push_back(item.key().c_str());
        }
        break; // Eerste key is genoeg
    }

    std::sort(langs.begin(), langs.end());
    langs.erase(std::unique(langs.begin(), langs.end()), langs.end());

    return langs;
}

//  --  getCurrentLanguage  --  //
String LanguageManager::getCurrentLanguage() const
{
    return currentLang;
}

//  --  init  --  //
bool LanguageManager::init(const String &defaultLangPath)
{
    if (!LittleFS.begin())
    {
        DEBUG_LOG("❌ LittleFS mounten mislukt");
        return false;
    }

    String loadedLang = EEPROM_Manager::loadLanguageFromEEPROM();
    if (loadedLang.isEmpty())
    {
        loadedLang = defaultLang;
        DEBUG_LOG("⚠️ Geen taalcode in EEPROM, fallback naar 'nl'");
    }

    setLanguage(loadedLang);

    if (!loadTranslationsFromJson(defaultLangPath))
    {
        DEBUG_LOG("❌ Kan vertalingen niet laden");
        return false;
    }

    DEBUG_LOGF("🌐 Taal geladen en ingesteld op: %s", loadedLang.c_str());
    return true;
}
