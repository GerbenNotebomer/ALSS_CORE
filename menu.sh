#!/bin/bash

while true; do
  echo "=== ESP32-S2 PlatformIO Menu ==="
  echo "0) Exit"
  echo "1) Clean project"
  echo "2) Build project"
  echo "3) Build and Upload project"
  echo "4) Upload LittleFS data"
  echo "5) Push project naar GitHub"
  read -p "Kies een optie (0-5): " choice

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

      # Detecteer huidige branch
      current_branch=$(git rev-parse --abbrev-ref HEAD)
      echo "Huidige git branch: $current_branch"

      # Alles toevoegen en committen (ook als er geen wijziging is)
      git add .
      git commit -m "Forced update via script" --allow-empty

      # Force push naar huidige branch
      git push origin "$current_branch" --force

      echo "Push voltooid."
      ;;
    *)
      echo "Ongeldige optie. Probeer het opnieuw."
      ;;
  esac

  echo ""  # lege regel voor overzichtelijkheid
done
