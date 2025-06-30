#!/bin/bash

# Controleer of een zoekterm is meegegeven
if [ -z "$1" ]; then
    echo "Gebruik: $0 \"zoekterm\" [directory]"
    exit 1
fi

ZOEKTERM="$1"
DIR="${2:-.}"  # Als geen dir is opgegeven, gebruik huidige dir

echo "Zoeken naar: \"$ZOEKTERM\" in directory: \"$DIR\""
echo "-----------------------------------------------"

# Gebruik grep met:
# -r : recursief
# -n : toon regelnummer
# -H : toon bestandsnaam
# --color=always : markeer zoekterm
# -I : negeer binaire bestanden

grep -rInH --color=always "$ZOEKTERM" "$DIR"

