#pragma once

#include <list>
#include "SpriteSheetAnimation.h"
#include "MathHelper.h"
#include "EventManager.h"

#define RAD 6.28318530718 // 2*PI

// Effekttypen
#define FX_SPARKS 0
#define FX_FIRE 1
#define FX_TRANSIT 2
#define FX_PARTICLEFIELD 3

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

class sparkle {
public:
    bool active;
    GLfloat size;
    GLfloat ang;
    int life;
    int lifeleft;
    position p, v;
    effect_vars vars;
    GLfloat bounce, f;

    sparkle();
    void draw(const float deltaTime);
};

class EffectsTransist {
    GLfloat opacity;
public:
    int age;
    effect_vars vars;

    EffectsTransist();
    void init();
    void draw(const float deltaTime);
};

class particleFieldClass {
    int spawnTimeout;
    sparkle* sparks;

    void spawnSpark(int sparkNum);

public:
    position p;
    effect_vars vars;

    void init(effect_vars varsP, position p);
    void draw(const float deltaTime);
    void move(position p);

    ~particleFieldClass();
};

class effect_class {
    position spawn_pos;
    sparkle* sparks;

public:
    EffectsTransist transit;
    particleFieldClass* pf;
    effect_vars vars;

    effect_class();
    void init(position p);
    void draw(const float deltaTime);
};

class EffectManager {
    effect_vars vars;
    int effectId;
    EventManager* eventManager;

public:
    std::list<effect_class> effects;

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

    // Event-Handler für verschiedene Spielereignisse
    void handleBallPaddleCollision(const EventData& data);
    void handleBallBrickCollision(const EventData& data);
    void handleBrickDestroyed(const EventData& data);
    void handlePowerUpCollected(const EventData& data);

    int isActive(int id) const;
    void kill(int id);

private:
    void registerEventListeners();
};
