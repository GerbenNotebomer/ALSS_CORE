#!/bin/bash

# Vast pad naar jouw Python 3.13 executable
PYTHON_EXEC="C:/Users/gknot/AppData/Local/Programs/Python/Python313/python.exe"

while true; do
  echo "=== ESP32-S2 PlatformIO Menu ==="
  echo "0) Exit"
  echo "1) Clean project"
  echo "2) Build project"
  echo "3) Build and Upload project"
  echo "4) Upload LittleFS data"
  echo "5) Push project naar GitHub"
  echo "6) Vertalingen genereren (ODS → JSON)"
  echo "7) Vertalingen deployen naar ESP32 (data/lang)"
  read -p "Kies een optie (0-7): " choice

  case $choice in
    0)
      echo "Exit."
      exit 0
      ;;
    1)
      echo "Project schoonmaken..."
      pio run --target clean
      ;;
    2)
      echo "Project compileren..."
      pio run
      ;;
    3)
      echo "Project compileren en uploaden..."
      pio run --target upload
      ;;
    4)
      echo "LittleFS data uploaden..."
      pio run --target uploadfs
      ;;
    5)
      echo "Pushen naar GitHub..."
      current_branch=$(git rev-parse --abbrev-ref HEAD)
      echo "Huidige git branch: $current_branch"
      git add .
      git commit -m "Forced update via script" --allow-empty
      git push origin "$current_branch" --force
      echo "Push voltooid."
      ;;
    6)
      echo "Vertalingen genereren vanuit ODS..."
      "$PYTHON_EXEC" src/language/translationtools/ods_to_json_translations.py
      ;;
    7)
      echo "Vertalingen deployen naar ESP32 map..."
      "$PYTHON_EXEC" src/language/translationtools/deploy_translations.py
      ;;
    *)
      echo "Ongeldige optie. Probeer het opnieuw."
      ;;
  esac

  echo ""  # lege regel voor overzicht
done
