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
#define DEFAULT_KEY_LEFT       SDLK_LEFT
#define DEFAULT_KEY_RIGHT      SDLK_RIGHT
#define DEFAULT_KEY_SHOOT      SDLK_UP
#define DEFAULT_KEY_NEXT_PO    SDLK_v
#define DEFAULT_KEY_BUY_PO     SDLK_b
#define DEFAULT_KEY_PREV_PO    SDLK_n

#define DEFAULT_CONTROL_ACCEL  7
#define DEFAULT_CONTROL_START_SPEED 1.0f
#define DEFAULT_CONTROL_MAX_SPEED   5

// Default Joystick Settings
#define DEFAULT_JOY_ENABLED    0
#define DEFAULT_JOY_IS_DIGITAL 0
#define DEFAULT_JOY_IS_PADDLE  0
#define DEFAULT_SHOW_CLOCK     1

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

#define PI 3.14159265
#define RAD 6.28318531

#define BOUNCE_ANGLE_RANGE 2.61799388 // // 150 Grad in Radiant
#define MIN_BOUNCE_ANGLE 0.261799388 // 15 Grad in Radiant

#define DEFAULT_DIFFICULTY    EASY


#define BRICK_WIDTH 0.0719f
#define BRICK_HEIGHT 0.0575f

#define PILLAR_WIDTH 0.038f
// Von -1 bis +1 in NDC, direkt Stein an Stein
// glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2
//         +1
//         ^
//         |
// -1 <----+----> +1
//         |
//        -1
#define PLAYFIELD_WIDTH (2.0f - (PILLAR_WIDTH * 2.0f))
#define PLAYFIELD_LEFT_BORDER (-1.0f + PILLAR_WIDTH)
#define PLAYFIELD_RIGHT_BORDER (1.0f - PILLAR_WIDTH)
