// EffectManager.cpp
#include "EffectManager.h"
#include <SDL2/SDL_log.h>

// --- EffectManager Implementierung ---

EffectManager::EffectManager(EventManager *eventMgr) : eventManager(eventMgr) {
    effects.clear();
    effectId = 0;

    registerEventListeners();
}

EffectManager::~EffectManager() {
    effects.clear();

    // Lösche alle Tracer
    for (auto &pair: tracers) {
        delete pair.second;
    }
    tracers.clear();
    objectTracers.clear();
}

void EffectManager::registerEventListeners() {
    // Ball-Bewegungsereignisse
    eventManager->addListener(GameEvent::BallCreated,
                              [this](const EventData &data) { handleObjectTracerCreate(data); }, this);

    eventManager->addListener(GameEvent::BallMoved,
                              [this](const EventData &data) { handleObjectTracerUpdate(data); }, this);

    eventManager->addListener(GameEvent::BallDestroyed,
                              [this](const EventData &data) { handleObjectTracerRemove(data); }, this);

    // Event-Handler für Kollisionen registrieren
    eventManager->addListener(GameEvent::BallHitPaddle,
                              [this](const EventData &data) { handleBallPaddleCollision(data); }, this);

    eventManager->addListener(GameEvent::BallHitBrick,
                              [this](const EventData &data) { handleBallBrickCollision(data); }, this);

    eventManager->addListener(GameEvent::BrickDestroyed,
                              [this](const EventData &data) { handleBrickDestroyed(data); }, this);

    eventManager->addListener(GameEvent::PowerUpCollected,
                              [this](const EventData &data) { handlePowerUpCollected(data); }, this);
}

void EffectManager::set(int var, GLfloat val) {
    switch (var) {
        case FX_VAR_SPEED:
            effectProperties.speed = val;
            break;
        case FX_VAR_SPREAD:
            effectProperties.spread = val;
            break;
        case FX_VAR_SIZE:
            effectProperties.size = val;
            break;
        case FX_VAR_GRAVITY:
            effectProperties.gravity = val;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, int val) {
    switch (var) {
        case FX_VAR_NUM:
            effectProperties.num = val;
            break;
        case FX_VAR_LIFE:
            effectProperties.life = val;
            break;
        case FX_VAR_TYPE:
            effectProperties.type = val;
            break;
        case FX_VAR_COLDET:
            effectProperties.collisionDetection = val;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, GLfloat r, GLfloat g, GLfloat b) {
    switch (var) {
        case FX_VAR_COLOR:
            effectProperties.col[0] = r;
            effectProperties.col[1] = g;
            effectProperties.col[2] = b;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, SpriteSheetAnimation tex) {
    switch (var) {
        case FX_VAR_TEXTURE:
            effectProperties.texture = tex;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, position p) {
    switch (var) {
        case FX_VAR_RECTANGLE:
            effectProperties.rect = p;
            break;
        default:
            break;
    }
}

int EffectManager::spawn(position p) {
    effectId++;
    effect_class tempEffect;
    effectProperties.effectId = effectId;
    tempEffect.effectProperties = effectProperties;
    tempEffect.init(p);
    effects.push_back(tempEffect);

    return effectId;
}

// Draw all effects with deltaTime parameter
void EffectManager::draw(const float deltaTime) {
    for (auto it = effects.begin(); it != effects.end();) {
        it->draw(deltaTime);

        if (!it->effectProperties.active) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
    // Zeichne nur aktive Tracer
    for (auto &pair: tracers) {
        Tracer *tracer = pair.second;
        if (tracer->isActive()) {
            // Benutze die GameObject-Methode isActive()
            tracer->draw();
        }
    }
}

int EffectManager::isActive(const int id) const {
    for (const auto &effect: effects) {
        if (effect.effectProperties.effectId == id && effect.effectProperties.active) {
            return 1;
        }
    }
    return 0;
}

void EffectManager::kill(int id) {
    for (auto &effect: effects) {
        if (effect.effectProperties.effectId == id) {
            effect.effectProperties.active = false;
        }
    }
}

// Event-Handler-Implementierungen

void EffectManager::handleBallPaddleCollision(const EventData &data) {
    // Funkeneffekt bei Paddle-Kollision erzeugen
    set(FX_VAR_TYPE, FX_SPARKS);
    set(FX_VAR_NUM, 15);
    set(FX_VAR_LIFE, 300);
    set(FX_VAR_SPEED, 0.005f);
    set(FX_VAR_SIZE, 0.025f);
    set(FX_VAR_COLOR, 0.7f, 0.7f, 1.0f);

    position p = {data.posX, data.posY};
    spawn(p);
}

void EffectManager::handleBallBrickCollision(const EventData &data) {
    // Kleinerer Effekt bei Brick-Kollision
    set(FX_VAR_TYPE, FX_SPARKS);
    set(FX_VAR_NUM, 8);
    set(FX_VAR_LIFE, 200);
    set(FX_VAR_SPEED, 0.003f);
    set(FX_VAR_SIZE, 0.015f);
    set(FX_VAR_COLOR, 1.0f, 0.7f, 0.3f);

    position p = {data.posX, data.posY};
    spawn(p);
}

void EffectManager::handleBrickDestroyed(const EventData &data) {
    // Größerer Explosion-Effekt bei Zerstörung eines Bricks
    set(FX_VAR_TYPE, FX_SPARKS);
    set(FX_VAR_NUM, 25);
    set(FX_VAR_LIFE, 500);
    set(FX_VAR_SPEED, 0.008f);
    set(FX_VAR_SIZE, 0.03f);
    set(FX_VAR_COLOR, 1.0f, 0.5f, 0.2f);

    position p = {data.posX, data.posY};
    spawn(p);
}

void EffectManager::handlePowerUpCollected(const EventData &data) {
    // Spezialeffekt für PowerUp-Sammlung
    set(FX_VAR_TYPE, FX_PARTICLEFIELD);
    set(FX_VAR_NUM, 40);
    set(FX_VAR_LIFE, 800);
    set(FX_VAR_SPEED, 0.006f);
    set(FX_VAR_SIZE, 0.02f);
    set(FX_VAR_COLOR, 0.3f, 1.0f, 0.3f);

    position p = {data.posX, data.posY};
    spawn(p);
}


int EffectManager::createTracer(float width, float height, bool explosive) {
    int id = ++effectId;
    auto *tracer = new Tracer();
    tracer->setSize(width, height); // Statt direktem Zugriff die Methode verwenden
    //tracer->texture = tracerTexture;

    // Standardfarben für normale/explosive Tracer setzen
    GLfloat defaultColors[3] = {0.7f, 0.7f, 1.0f}; // Blau für normale Tracer
    tracer->colorRotate(explosive, explosive ? nullptr : defaultColors);

    tracers[id] = tracer;
    return id;
}

void EffectManager::updateTracer(int tracerId, float x, float y) {
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        it->second->updatePosition(x, y); // Die überarbeitete Methode verwenden
    }
}

void EffectManager::setTracerColor(int tracerId, bool explosive, const GLfloat c[]) {
    if (auto it = tracers.find(tracerId); it != tracers.end()) {
        it->second->colorRotate(explosive, c);
    }
}

void EffectManager::setTracerSize(int tracerId, float width, float height) {
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        it->second->setSize(width, height); // Die dedizierte Methode verwenden
    }
}

void EffectManager::removeTracer(int tracerId) {
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        delete it->second;
        tracers.erase(it);
    }
}

void EffectManager::setTracerTexture(int tracerId, SpriteSheetAnimation texture) {
    if (const auto it = tracers.find(tracerId); it != tracers.end()) {
        it->second->texture = &texture;
    }
}

void EffectManager::setTracerActive(int tracerId, bool active) {
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        GameObject *go = dynamic_cast<GameObject *>(it->second);
        if (go) {
            go->active = active;
        }
    }
}

// Handler für die Erstellung eines Tracers für ein Objekt
void EffectManager::handleObjectTracerCreate(const EventData &data) {
    void *object = data.sender;
    bool isExplosive = (data.points != 0);
    int tracerId = createTracer(0.015f, 0.015f, isExplosive);
    int objectId = reinterpret_cast<uintptr_t>(object);
    objectTracers[objectId] = tracerId;
    updateTracer(tracerId, data.posX, data.posY);
}

// Handler für die Aktualisierung eines Tracers
void EffectManager::handleObjectTracerUpdate(const EventData &data) {
    void *object = data.sender;
    int objectId = reinterpret_cast<uintptr_t>(object);
    auto it = objectTracers.find(objectId);

    if (it != objectTracers.end()) {
        updateTracer(it->second, data.posX, data.posY);
    }
}

// Handler für das Entfernen eines Tracers
void EffectManager::handleObjectTracerRemove(const EventData &data) {
    void *object = data.sender;
    int objectId = reinterpret_cast<uintptr_t>(object);
    auto it = objectTracers.find(objectId);

    if (it != objectTracers.end()) {
        removeTracer(it->second);
        objectTracers.erase(it);
    }
}
