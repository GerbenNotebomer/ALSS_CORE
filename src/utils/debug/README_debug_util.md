# DebugUtil Module

Deze module verzorgt uitgebreide, modulair aanpasbare debugging en logging voor Arduino-achtige projecten.

---

## Functionaliteit

- Modulair in- en uitschakelen van debugging per bronbestand/module.
- Bufferen van de laatste 100 logregels in het geheugen.
- Logging met printf-achtige syntax (`DEBUG_LOGF`), plus eenvoudige string logging.
- Optioneel in- en uitschakelen van logging per module.
- Logging output naar `Serial` poort.
- Handige functies om logs op te halen, te printen en te wissen.

---

## Gebruik

1. **Initialisatie** (optioneel):
    ```cpp
    DebugUtil::initDefaultModules();
    ```

2. **Globale debug aan/uit zetten:**
    ```cpp
    DebugUtil::setEnabled(true); // of false
    ```

3. **Debug aan/uit zetten per module (bestand):**
    ```cpp
    DebugUtil::setModuleEnabled("thing_speak.cpp", true);
    ```

4. **Logging gebruiken met macro's:**
    ```cpp
    DEBUG_LOG("Een simpele logregel");
    DEBUG_LOGF("Waarde: %d", waarde);
    DEBUG_PRINT("Print zonder newline");
    DEBUG_PRINTLN("Print met newline");
    ```

5. **Logs ophalen en wissen:**
    ```cpp
    String logs;
    DebugUtil::getLogBuffer(logs);
    Serial.println(logs);

    DebugUtil::clearLogBuffer();
    ```

---

## Macro's

- `DEBUG_LOGF(fmt, ...)` — printf-stijl log met bestandsnaam.
- `DEBUG_LOG(msg)` — eenvoudige string log.
- `DEBUG_PRINT(msg)` — print zonder newline.
- `DEBUG_PRINTLN(msg)` — print met newline.

---

## Intern

- Houdt maximaal 100 logregels bij in `logBuffer`.
- Modules worden geïdentificeerd op bestandsnaam (bijv. `"thing_speak.cpp"`).
- Nieuwe modules worden automatisch toegevoegd bij eerste gebruik.
- Debug output gaat naar `Serial`.

---

## Voorbeeld

```cpp
#include "debug_util.h"

void setup() {
    Serial.begin(115200);
    DebugUtil::initDefaultModules();

    DEBUG_LOG("Setup gestart");
    DEBUG_LOGF("Waarde van x = %d", 42);
}

void loop() {
    DEBUG_PRINT("Loop actief...");
    delay(1000);
}

Bestanden
debug_util.cpp — implementatie van debugfunctionaliteit.

debug_util.h — declaraties en macro's.

