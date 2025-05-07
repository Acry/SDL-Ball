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
#define CONFIG_FILE        "settings.cfg"
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
#define GAME_AUDIO_CHANNELS    16 // I have 8
#define USED_SOUND_SAMPLES     25

// Sound Samples
#define SND_START 0
#define SND_BALL_HIT_BORDER 1
#define SND_BALL_HIT_PADDLE 2
#define SND_NORM_BRICK_BREAK 3
#define SND_EXPL_BRICK_BREAK 4
#define SND_GLASS_BRICK_HIT 5
#define SND_GLASS_BRICK_BREAK 6
#define SND_CEMENT_BRICK_HIT 7
#define SND_PO_HIT_BORDER 8
#define SND_GOOD_PO_HIT_PADDLE 9
#define SND_EVIL_PO_HIT_PADDLE 10
#define SND_SHOT 11
#define SND_DIE 12
#define SND_NEXTLEVEL 13
#define SND_GAMEOVER 14
#define SND_MENUCLICK 15
#define SND_DOOM_BRICK_BREAK 16
#define SND_GLUE_BALL_HIT_PADDLE 17
#define SND_INVISIBLE_BRICK_APPEAR 18
#define SND_HIGHSCORE 19
#define SND_BUY_POWERUP 20
#define SND_NORM_BRICK_BREAKB 21
#define SND_NORM_BRICK_BREAKC 22
#define SND_NORM_BRICK_BREAKD 23
#define SND_NORM_BRICK_BREAKE 24

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

// Default Effect Settings
#define FX_SPARKS 0
#define FX_FIRE 1
#define FX_TRANSIT 2
#define FX_PARTICLEFIELD 3

#define FX_VAR_TYPE 1

#define FX_VAR_NUM 0
#define FX_VAR_SPEED 2
#define FX_VAR_SPREAD 3
#define FX_VAR_SIZE 4
#define FX_VAR_LIFE 5
#define FX_VAR_GRAVITY 6
#define FX_VAR_COLDET 7
#define FX_VAR_COLOR 8
#define FX_VAR_TEXTURE 9
#define FX_VAR_RECTANGLE 10
