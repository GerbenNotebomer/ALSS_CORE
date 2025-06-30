# 📘 Handleiding: Parameter Toevoegen aan Parametersysteem

Deze handleiding legt uit hoe je een **nieuwe parameter** toevoegt aan het parametersysteem in je project.

---

## 🧩 Structuuroverzicht van het Parametersysteem

Je systeem bestaat uit:
1. `Parameters` namespace met alle waarden
2. `ParameterDescriptor` struct (metadata: key + type + pointer)
3. `getAllParameters()` - lijst van alle parameters
4. Functies om parameters te laden/schrijven van/naar SD-kaart (`/config/parameters.txt`)
5. Automatisch aanvullen van ontbrekende parameters met standaardwaarden

---

## 🛠️ Stap-voor-stap: Nieuwe Parameter Toevoegen

### ✏️ Voorbeeld:
We willen de volgende parameter toevoegen:

```cpp
int maxLogSize = 5000;
```

---

### ✅ 1. Voeg toe aan `Parameters.h`:

```cpp
extern int maxLogSize;
```

---

### ✅ 2. Definieer in `Parameters.cpp`:

```cpp
int maxLogSize = 5000;
```

---

### ✅ 3. Voeg toe aan `getAllParameters()`:

```cpp
{"maxLogSize", &Parameters::maxLogSize, ParameterDescriptor::INT},
```

Bijv.:

```cpp
{"smoothingWarmupThreshold", &Parameters::smoothingWarmupThreshold, ParameterDescriptor::INT},
{"maxLogSize", &Parameters::maxLogSize, ParameterDescriptor::INT},
```

---

### ✅ 4. Voeg toe aan `setDefaultParameters()`:

```cpp
Parameters::maxLogSize = 5000;
```

---

## ✅ Resultaat

Bij het opstarten:
- Als `/config/parameters.txt` de parameter **niet bevat**, wordt deze **automatisch toegevoegd met default**.
- Als hij **wel bestaat**, wordt de waarde geladen vanaf SD-kaart.

---

## 📄 Voorbeeldregel in `parameters.txt`:

```txt
maxLogSize=5000
```

Je kunt dit bestand handmatig bewerken óf automatisch laten aanvullen.

---

## ⚠️ Veelgemaakte Fouten & Tips

| Probleem | Oplossing |
|---------|-----------|
| ❌ `extern` vergeten in `.h` | Voeg extern-declaratie toe in `Parameters.h` |
| ❌ Niet toegevoegd aan `getAllParameters()` | Parameter wordt niet verwerkt |
| ❌ Verkeerd type (bijv. `LONG` vs `INT`) | Kies juiste `ParameterDescriptor::Type` |
| ❌ SD-kaartbestand ontbreekt | `initParameters()` maakt automatisch `parameters.txt` aan |

---

## 🧪 Test Tip

Verwijder `/config/parameters.txt` om te testen of de parameter correct aangemaakt wordt bij defaultinitialisatie.

---

© 2025 — Parametersysteem voor ESP32
