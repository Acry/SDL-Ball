// EffectManager.h
#pragma once

#include <list>
#include "MathHelper.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Tracer.h"
#include "EffectDefinitions.h"


class Fade {
    GLfloat opacity;

public:
    int age;
    EffectProperties effectProperties;

    Fade();

    void init();

    void draw(const float deltaTime);
};

class Particles {
    int spawnTimeout;
    Sparkle *sparks;

    void spawnSpark(int sparkNum);

public:
    position p;
    EffectProperties vars;

    void init(EffectProperties varsP, position p);

    void draw(const float deltaTime);

    void move(position p);

    ~Particles();
};

class effect_class {
    position spawn_pos;
    Sparkle *sparks;

public:
    Fade transit;
    Particles *pf;
    EffectProperties effectProperties;

    effect_class();

    void init(position p);

    void draw(const float deltaTime);
};

class EffectManager {
    std::list<effect_class> effects;
    EffectProperties effectProperties;
    int effectId;
    EventManager *eventManager;

    // Verwaltung der Tracer-Effekte
    std::unordered_map<int, Tracer *> tracers;
    SpriteSheetAnimation tracerTexture;

public:
    EffectManager(EventManager *eventMgr);

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
    // BallManager sendet BallCreated, BallMoved und BallDestroyed Events
    std::unordered_map<int, int> objectTracers;

    void handleBallPaddleCollision(const EventData &data);

    void handleBrickDestroyed(const EventData &data);

    void handlePowerUpCollected(const EventData &data);

    void handleBallBrickCollision(const EventData &data);
};
