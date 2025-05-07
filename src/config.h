#pragma once

// SDL Ball Configuration Header
/*
  * Enthält compile-time Definitionen und Konstanten
  * Wird zum Build-Zeitpunkt ausgewertet
  * Typischerweise für projektweite Einstellungen und Funktionen
*/
// Versions

// Default Game meta-information
#define VERSION "3.1"
#define SAVEGAME_VERSION 2
#define GAME_NAME "SDL-Ball"

// Game Difficulties
#define EASY   0
#define NORMAL 1
#define HARD   2

#define DEFAULT_DIFFICULTY    EASY

// Dateipfade und Verzeichnisse
#define CONFIG_FILE        "config.cfg"
#define SAVEGAME_FILE      "savegame.sav"
#define HIGHSCORE_FILE     "highscores.txt"
#define SCREENSHOT_DIR     "screenshots"

// Default Visuals
#define DEFAULT_GFX_THEME      "default"
#define DEFAULT_SND_THEME      "default"
#define DEFAULT_LVL_THEME      "default"

#define DEFAULT_EYE_CANDY      1
#define DEFAULT_SHOW_BG        1
#define DEFAULT_PARTICLE_COLLIDE 1

// Default Audio Settings
#define DEFAULT_SOUND          1
#define DEFAULT_STEREO         1
#define SNDSAMPLES             25

// Default Control Settings
#define DEFAULT_KEY_LEFT       276
#define DEFAULT_KEY_RIGHT      275
#define DEFAULT_KEY_SHOOT      273
#define DEFAULT_KEY_NEXT_PO    SDLK_v
#define DEFAULT_KEY_BUY_PO     SDLK_b
#define DEFAULT_KEY_PREV_PO    SDLK_n

#define DEFAULT_CONTROL_ACCEL  7
#define DEFAULT_CONTROL_START_SPEED 1.0f
#define DEFAULT_CONTROL_MAX_SPEED   5

// Default Joystick Settings
#define DEFAULT_JOY_ENABLED    1
#define DEFAULT_JOY_IS_DIGITAL 1
#define DEFAULT_JOY_IS_PADDLE  0
#define DEFAULT_SHOW_CLOCK     0

// Default Joystick Calibration
#define DEFAULT_JOY_CAL_MIN    (-32767)
#define DEFAULT_JOY_CAL_MAX     32767
#define DEFAULT_JOY_CAL_LOW     (-20)
#define DEFAULT_JOY_CAL_HIGH     20

// Default Display Settings
#define DEFAULT_FPS             60
#define DEFAULT_FULLSCREEN      0
#define DEFAULT_RES_X           1920
#define DEFAULT_RES_Y           1080
