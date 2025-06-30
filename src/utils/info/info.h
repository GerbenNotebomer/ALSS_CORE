#pragma once

#include <Arduino.h>

// Geeft board naam terug
String getBoardName();

// Geeft een overzicht van systeeminformatie terug als HTML-string
String getSystemInfo();

void checkPsram();