// EffectManager.cpp
#include "EffectManager.h"
#include <SDL2/SDL_log.h>

// The 'sparkle' class represents a single particle used for spark and particle field effects.
// Each sparkle has its own position, velocity, size, color, and lifetime.
// It simulates simple physics (gravity, friction, bounce) and fades out over time.
// Used for visual feedback like collisions or explosions.
Sparkle::Sparkle() {
    bounce = 0;
    f = 0;
    active = true;
}

void Sparkle::draw(const float deltaTime) {
    if (lifeleft > 0) {
        // Update lifetime
        lifeleft -= (int) (deltaTime * 1000.0f);

        // Deactivate if out of visible area
        if (p.x > 1.67 || p.y < -1.7 || p.x < -1.67) {
            active = false;
        }

        // Update physics (gravity, bounce, friction)
        v.y -= vars.gravity * deltaTime;
        v.y -= bounce * deltaTime;

        if (v.x < 0) {
            v.x += f * deltaTime;
        } else {
            v.x -= f * deltaTime;
        }

        // Update position
        p.x += v.x * deltaTime;
        p.y += v.y * deltaTime;

        // Draw the sparkle as a textured quad, fading out over its lifetime
        glColor4f(vars.col[0], vars.col[1], vars.col[2],
                  (1.0f / static_cast<float>(life)) * static_cast<float>(lifeleft));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Für normale Transparenz
        // Oder für additive Blending (leuchtende Effekte):
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        const GLfloat curSize = size / static_cast<float>(life) * static_cast<float>(lifeleft);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, vars.tex.textureProperties.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(p.x - curSize, p.y + curSize, 0.0);
        glTexCoord2f(0, 1);
        glVertex3f(p.x + curSize, p.y + curSize, 0.0);
        glTexCoord2f(1, 1);
        glVertex3f(p.x + curSize, p.y - curSize, 0.0);
        glTexCoord2f(1, 0);
        glVertex3f(p.x - curSize, p.y - curSize, 0.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    } else {
        active = false;
    }
}

// The 'EffectsTransist' class implements a screen transition effect (fade in/out).
// It draws a colored quad over the screen and animates its opacity for smooth transitions.
// Used for scene changes or level transitions.
Fade::Fade() {
    opacity = 0.0f;
    age = 0;
}

void Fade::init() {
    age = 0;
    opacity = 0.0f;
    vars.transition_half_done = 0;
}

void Fade::draw(const float deltaTime) {
    age += (int) (deltaTime * 1000.0f);

    float progress = std::min(1.0f, (float) age / (float) vars.life);

    if (progress < 0.5f) {
        opacity = progress * 2.0f; // 0->1 in erster Hälfte
    } else {
        if (!vars.transition_half_done) {
            vars.transition_half_done = 1;
        }
        opacity = 1.0f - ((progress - 0.5f) * 2.0f); // 1->0 in zweiter Hälfte
    }

    opacity = std::max(0.0f, std::min(1.0f, opacity));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    glColor4f(vars.col[0], vars.col[1], vars.col[2], opacity);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0);
    glVertex3f(1.0f, 1.0f, 0.0);
    glVertex3f(1.0f, -1.0f, 0.0);
    glVertex3f(-1.0f, -1.0f, 0.0);
    glEnd();
    glDisable(GL_BLEND);
}

// The 'particleFieldClass' manages a group of sparkles to create a particle field effect.
// It spawns multiple sparkles from a given position, each with random direction and properties.
// Used for effects like power-up collection or special bursts.
void Particles::init(effect_vars varsP, position spawnPos) {
    vars = varsP;
    spawnTimeout = 0;
    vars.active = true;
    sparks = new Sparkle[vars.num];
    p = spawnPos;

    for (int i = 0; i < vars.num; i++) {
        sparks[i].active = true;
        sparks[i].vars = vars;
        spawnSpark(i);
    }
}

Particles::~Particles() {
    delete[] sparks; // Free memory
}

void Particles::spawnSpark(int sparkNum) {
    // Spawn a single sparkle with random angle, speed, and position offset
    const GLfloat angle = (RAD / vars.num - 1) * (random_float(vars.num, 0.0));
    spawnTimeout = rand() % 10;
    sparks[sparkNum].life = rand() % vars.life;
    sparks[sparkNum].lifeleft = sparks[sparkNum].life;
    sparks[sparkNum].v.x = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * sin(angle);
    sparks[sparkNum].v.y = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * cos(angle);
    sparks[sparkNum].size = random_float(vars.size, 0);

    sparks[sparkNum].p = p;

    sparks[sparkNum].p.x += random_float(vars.rect.x, (vars.rect.x / 2.0));
    sparks[sparkNum].p.y += random_float(vars.rect.y, (vars.rect.y / 2.0));
    sparks[sparkNum].active = true;
}

void Particles::draw(const float deltaTime) {
    spawnTimeout -= (int) (deltaTime * 1000.0f);

    int t = 0;

    for (int i = 0; i < vars.num; i++) {
        if (sparks[i].active) {
            t++;
            sparks[i].draw(deltaTime);
        }
    }

    vars.life -= (int) (deltaTime * 1000.0f);
    if (t == 0) {
        vars.active = 0;
    }
}

void Particles::move(position newPos) {
    p = newPos;
}

// The 'effect_class' acts as a wrapper for different effect types (sparks, transition, particle field).
// It initializes and manages the correct effect based on the type and delegates drawing.
effect_class::effect_class() {
    vars.active = false; // Not active on creation
    pf = nullptr;
    sparks = nullptr;
}

void effect_class::init(position p) {
    vars.active = true;
    spawn_pos = p;

    switch (vars.type) {
        case FX_SPARKS:
            // Initialize an array of sparkles for the spark effect
            sparks = new Sparkle[vars.num];
            if (sparks == nullptr) {
                SDL_Log("Konnte %d Funken nicht allokieren", vars.num);
                return;
            }

            for (int i = 0; i < vars.num; i++) {
                sparks[i].size = random_float(vars.size, 0);
                int life = rand() % vars.life;
                float angle = (RAD / vars.num - 1) * (random_float(vars.num, 0.0));

                sparks[i].life = life;
                sparks[i].lifeleft = life;
                sparks[i].v.x = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * sin(angle);
                sparks[i].v.y = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * cos(angle);

                sparks[i].vars = vars;
                sparks[i].p = spawn_pos;
            }
            break;

        case FX_TRANSIT:
            transit.vars = vars;
            transit.init();
            break;

        case FX_PARTICLEFIELD:
            pf = nullptr;
            pf = new Particles;
            pf->init(vars, p);
            break;
        default:
            break;
    }
}

void effect_class::draw(const float deltaTime) {
    bool stay = false;

    switch (vars.type) {
        case FX_SPARKS:
            for (int i = 0; i < vars.num; i++) {
                if (sparks[i].active) {
                    sparks[i].draw(deltaTime);
                    stay = true;
                }
            }
            break;

        case FX_TRANSIT:
            transit.draw(deltaTime);
            if (transit.age <= vars.life)
                stay = true;
            break;

        case FX_PARTICLEFIELD:
            pf->draw(deltaTime);
            if (pf->vars.active)
                stay = true;
            break;
        default:
            break;
    }

    // Verhindere Flackern bei Übergangseffekten
    if (vars.effectId != -1 && vars.type != FX_TRANSIT)
        stay = true;

    if (!stay) {
        vars.active = false;
        switch (vars.type) {
            case FX_SPARKS:
                delete[] sparks;
                break;

            case FX_PARTICLEFIELD:
                delete pf;
                break;
            default:
                break;
        }
    }
}

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
            vars.speed = val;
            break;
        case FX_VAR_SPREAD:
            vars.spread = val;
            break;
        case FX_VAR_SIZE:
            vars.size = val;
            break;
        case FX_VAR_GRAVITY:
            vars.gravity = val;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, int val) {
    switch (var) {
        case FX_VAR_NUM:
            vars.num = val;
            break;
        case FX_VAR_LIFE:
            vars.life = val;
            break;
        case FX_VAR_TYPE:
            vars.type = val;
            break;
        case FX_VAR_COLDET:
            vars.coldet = val;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, GLfloat r, GLfloat g, GLfloat b) {
    switch (var) {
        case FX_VAR_COLOR:
            vars.col[0] = r;
            vars.col[1] = g;
            vars.col[2] = b;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, SpriteSheetAnimation tex) {
    switch (var) {
        case FX_VAR_TEXTURE:
            vars.tex = tex;
            break;
        default:
            break;
    }
}

void EffectManager::set(int var, position p) {
    switch (var) {
        case FX_VAR_RECTANGLE:
            vars.rect = p;
            break;
        default:
            break;
    }
}

int EffectManager::spawn(position p) {
    effectId++;
    effect_class tempEffect;
    vars.effectId = effectId;
    tempEffect.vars = vars;
    tempEffect.init(p);
    effects.push_back(tempEffect);

    return effectId;
}

// Draw all effects with deltaTime parameter
void EffectManager::draw(const float deltaTime) {
    for (auto it = effects.begin(); it != effects.end();) {
        it->draw(deltaTime);

        if (!it->vars.active) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
    // Zeichne nur aktive Tracer
    for (auto &pair: tracers) {
        Tracer *tracer = pair.second;
        if (tracer->isActive()) {  // Benutze die GameObject-Methode isActive()
            tracer->draw(deltaTime);
        }
    }
}

int EffectManager::isActive(const int id) const {
    for (const auto &effect: effects) {
        if (effect.vars.effectId == id && effect.vars.active) {
            return 1;
        }
    }
    return 0;
}

void EffectManager::kill(int id) {
    for (auto &effect: effects) {
        if (effect.vars.effectId == id) {
            effect.vars.active = false;
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
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        it->second->texture = texture;
    }
}

void EffectManager::setTracerActive(int tracerId, bool active) {
    auto it = tracers.find(tracerId);
    if (it != tracers.end()) {
        GameObject* go = dynamic_cast<GameObject*>(it->second);
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
