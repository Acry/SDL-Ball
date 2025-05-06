#pragma once

// SDL Ball Configuration Header
/*
  * Enthält compile-time Definitionen und Konstanten
  * Wird zum Build-Zeitpunkt ausgewertet
  * Typischerweise für projektweite Einstellungen und Funktionen
*/
// Versions

#define VERSION "3.1"
#define SAVEGAMEVERSION 0x0104
#define GAME_NAME "SDL-Ball"
// Difficulties
#define EASY   0
#define NORMAL 1
#define HARD   2

// Dateipfade und Verzeichnisse
#define CONFIG_FILE         "config.cfg"
#define SAVEGAME_FILE      "savegame.sav"
#define HIGHSCORE_FILE     "highscores.txt"
#define SCREENSHOT_DIR     "screenshots"

// Verzeichnispfade relativ zum Programm-Root
#define CONFIG_PATH        "/" GAME_NAME "/" CONFIG_FILE
#define SAVEGAME_PATH     "/" GAME_NAME "/" SAVEGAME_FILE
#define HIGHSCORE_PATH    "/" GAME_NAME "/" HIGHSCORE_FILE
#define SCREENSHOT_PATH   "/" GAME_NAME "/" SCREENSHOT_DIR

// Hilfs-Makros zum Zusammensetzen der vollständigen Pfade
#define MAKE_PATH(root, path) ((root) + (path))
