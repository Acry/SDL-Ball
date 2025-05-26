#pragma once

#include <list>
#include "SpriteSheetAnimation.h"
#include "MathHelper.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Tracer.h"

#define RAD 6.28318530718 // 2*PI

// Effekttypen
#define FX_SPARKS 0
#define FX_FIRE 1
#define FX_TRANSIT 2
#define FX_PARTICLEFIELD 3
#define FX_TRACER 4

// Effekteigenschaften
#define FX_VAR_TYPE 1
#define FX_VAR_NUM 0
#define FX_VAR_LIFE 2
#define FX_VAR_SPEED 3
#define FX_VAR_SPREAD 4
#define FX_VAR_SIZE 5
#define FX_VAR_GRAVITY 6
#define FX_VAR_COLOR 7
#define FX_VAR_TEXTURE 8
#define FX_VAR_RECTANGLE 9
#define FX_VAR_COLDET 10

struct effect_vars {
    int type;
    int num;
    int life;
    float speed;
    float spread;
    float size;
    float gravity;
    float col[3];
    SpriteSheetAnimation tex;
    position rect;
    bool coldet;
    bool active;
    int effectId;
    int transition_half_done;
};

class Sparkle {
public:
    bool active;
    GLfloat size;
    GLfloat ang;
    int life;
    int lifeleft;
    position p, v;
    effect_vars vars;
    GLfloat bounce, f;

    Sparkle();
    void draw(const float deltaTime);
};

class Fade {
    GLfloat opacity;
public:
    int age;
    effect_vars vars;

    Fade();
    void init();
    void draw(const float deltaTime);
};

class Particles {
    int spawnTimeout;
    Sparkle* sparks;

    void spawnSpark(int sparkNum);

public:
    position p;
    effect_vars vars;

    void init(effect_vars varsP, position p);
    void draw(const float deltaTime);
    void move(position p);

    ~Particles();
};

class effect_class {
    position spawn_pos;
    Sparkle* sparks;

public:
    Fade transit;
    Particles* pf;
    effect_vars vars;

    effect_class();
    void init(position p);
    void draw(const float deltaTime);
};

class EffectManager {
    std::list<effect_class> effects;
    effect_vars vars;
    int effectId;
    EventManager* eventManager;

    // Verwaltung der Tracer-Effekte
    std::unordered_map<int, Tracer*> tracers;
    SpriteSheetAnimation tracerTexture;

public:
    EffectManager(EventManager* eventMgr);
    ~EffectManager();

    // Setter-Methoden für Effekteigenschaften
    void set(int var, GLfloat val);
    void set(int var, int val);
    void set(int var, GLfloat r, GLfloat g, GLfloat b);
    void set(int var, SpriteSheetAnimation tex);
    void set(int var, position p);

    // Effekt-Methoden
    int spawn(position p);
    void draw(const float deltaTime);

    int isActive(int id) const;
    void kill(int id);

    // Tracer-Verwaltungsmethoden
    int createTracer(float width, float height, bool explosive = false);
    void updateTracer(int tracerId, float x, float y);
    void setTracerColor(int tracerId, bool explosive, const GLfloat c[]);
    void setTracerSize(int tracerId, float width, float height);
    void removeTracer(int tracerId);
    void setTracerTexture(int tracerId, SpriteSheetAnimation texture);
    void setTracerActive(int tracerId, bool active);

    // Neue Event-Handler für Ball-Tracking
    void handleObjectTracerCreate(const EventData &data);
    void handleObjectTracerUpdate(const EventData &data);
    void handleObjectTracerRemove(const EventData &data);

private:
    void registerEventListeners();
    // Map für Ball-zu-Tracer Zuordnungen
    std::unordered_map<int, int> objectTracers;
    void handleBallPaddleCollision(const EventData& data);
    void handleBrickDestroyed(const EventData& data);
    void handlePowerUpCollected(const EventData& data);
    void handleBallBrickCollision(const EventData& data);
};
