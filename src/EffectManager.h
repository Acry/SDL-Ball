#pragma once

#include <list>
#include "SpriteSheetAnimation.h"
#include "TextManager.h"
#include "MathHelper.h"

#define RAD 6.28318530718 // 2*PI

// Am Anfang der EffectManager.h Datei
struct global_vars {
    bool transition_half_done;
    int effectnum;
};

// if (var.transition_half_done) {
//     var.titleScreenShow = true;
//     effectMananger.kill(var.effectnum);
//     var.effectnum = -1;
//     initNewGame();
//     resumeGame();
// }

extern global_vars vars;

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

#define MAXMSG 10

struct effect_vars {
    int type;            // Welche Art von Effekt
    bool coldet;         // Hat Kollisionserkennung
    bool active;         // Ist dieser Effekt aktiv
    GLfloat col[3];      // Farbe des Effekts
    GLfloat speed;       // Bewegungsgeschwindigkeit
    GLfloat spread;      // Streuung (in Grad)
    GLfloat size;        // Skalierung der Elemente
    position rect;       // Rechteckige Größe (für particleField)
    GLfloat gravity;     // Gravitationsstärke
    int num;             // Anzahl der Elemente
    int life;            // Lebensdauer in ms
    SpriteSheetAnimation tex;         // Textur
    int effectId;        // Eindeutige ID für diesen Effekt
};

// Transitionen-Effekt
class EffectsTransist {
    GLfloat opacity;
public:
    TextManager& text;
    int age;
    effect_vars vars;

    EffectsTransist();
    void init();
    void draw();
};

// Funken-Effekt
class sparkle {
public:
    bool active;
    GLfloat size;
    GLfloat ang;
    int life;
    int lifeleft;
    position p, v;       // Position und Geschwindigkeit
    effect_vars vars;
    GLfloat bounce, f;   // Abprallen und Reibung

    sparkle();
    void draw();
    void coldet(brick &b);
    void pcoldet(Paddle &b);
};

// Partikelfeld-Effekt
class particleFieldClass {
    int spawnTimeout;    // Zeitpunkt für zufälliges Spawnen von Funken
    sparkle *sparks;

    void spawnSpark(int sparkNum);

public:
    position p;
    effect_vars vars;

    void init(effect_vars varsP, position p);
    void draw();
    void move(position p);
    void coldet(brick &b);
    void pcoldet(Paddle &b);
    ~particleFieldClass();
};

// Effekt-Klasse für verschiedene Effekttypen
class effect_class {
    position spawn_pos;
    sparkle *sparks;

public:
    EffectsTransist transit;
    particleFieldClass *pf;
    effect_vars vars;

    effect_class();
    void init(position p);
    void draw();
    void coldet(brick &b);
    void pcoldet(Paddle &b);
};

// Hauptmanager für alle Effekte
class EffectManager {
    effect_vars vars;    // Wird auf den nächsten spawning Effekt kopiert
    int effectId;        // Fortlaufende Nummer für gespawnte Effekte

public:
    std::list<effect_class> effects;

    EffectManager();

    // Setzen verschiedener Effekteigenschaften
    void set(int var, GLfloat val);
    void set(int var, int val);
    void set(int var, GLfloat r, GLfloat g, GLfloat b);
    void set(int var, SpriteSheetAnimation tex);
    void set(int var, struct position p);

    // Effekt erzeugen und verwalten
    int spawn(position p);
    void draw();
    void coldet(brick &b);
    void pcoldet(Paddle &b);
    int isActive(int id);
    void kill(int id);
};

// Klasse für animierte Textankündigungen
class glAnnounceMessageClass {
    int age;
    GLfloat zoom, fade;
    bool fadedir;

public:
    TextManager& text;
    bool active;
    int life;
    std::string announcement;
    int font;

    glAnnounceMessageClass();
    void draw();
};

// Manager für Textankündigungen
class glAnnounceTextClass {
    int len;
    std::list<glAnnounceMessageClass> announcement;
    std::list<glAnnounceMessageClass>::iterator iterator;

public:
    glAnnounceTextClass();
    void write(const char *message, int ttl, int font);
    void draw();
};
