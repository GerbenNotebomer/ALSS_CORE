#!/bin/bash

while true; do
  echo "=== ESP32-S2 PlatformIO Menu ==="
  echo "1) Clean project"
  echo "2) Build project"
  echo "3) Build and Upload project"
  echo "4) Upload LittleFS data"
  echo "5) Exit"
  echo "6) Push project naar GitHub (exclusief parameters_private.cpp)"
  read -p "Kies een optie (1-6): " choice

  case $choice in
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
      echo "Exit."
      exit 0
      ;;
    6)
      echo "Pushen naar GitHub (exclusief parameters_private.cpp)..."

      # Detecteer huidige branch
      current_branch=$(git rev-parse --abbrev-ref HEAD)
      echo "Huidige git branch: $current_branch"

      # Stap 1: Verwijder tijdelijke kopie van parameters_private.cpp als die er is
      TMP_FILE="src/parameters/parameters_private.cpp.tmp_for_git_ignore"
      if [ -f "$TMP_FILE" ]; then
        rm "$TMP_FILE"
      fi

      # Stap 2: Maak een tijdelijke kopie (of verplaats) van parameters_private.cpp
      if [ -f "src/parameters/parameters_private.cpp" ]; then
        mv src/parameters/parameters_private.cpp "$TMP_FILE"
      fi

      # Stap 3: Voeg alles toe, commit en force push
      git add .
      git commit -m "Forced update via script (exclusief parameters_private.cpp)" --allow-empty
      git push origin "$current_branch" --force

      # Stap 4: Zet parameters_private.cpp terug
      if [ -f "$TMP_FILE" ]; then
        mv "$TMP_FILE" src/parameters/parameters_private.cpp
      fi

      echo "Push voltooid."
      ;;
    *)
      echo "Ongeldige optie. Probeer het opnieuw."
      ;;
  esac

  echo ""  # lege regel voor overzichtelijkheid
done
