// EffectManager.cpp
#include "EffectManager.h"

#include <SDL2/SDL_log.h>

extern int globalTicksSinceLastDraw;
extern float globalMilliTicksSinceLastDraw;

// --- sparkle Implementierung ---

sparkle::sparkle() {
    bounce = 0;
    f = 0;
    active = true;
}

void sparkle::draw() {
    if (lifeleft > 0) {
        // Lebensdauer aktualisieren
        lifeleft -= globalTicksSinceLastDraw;

        // Überprüfen, ob wir noch im sichtbaren Bereich sind
        if (p.x > 1.67 || p.y < -1.7 || p.x < -1.67) {
            active = false;
        }

        // Physik aktualisieren
        v.y -= vars.gravity * globalMilliTicksSinceLastDraw;
        v.y -= bounce * globalMilliTicksSinceLastDraw;

        if (v.x < 0) {
            v.x += f * globalMilliTicksSinceLastDraw;
        } else {
            v.x -= f * globalMilliTicksSinceLastDraw;
        }

        // Position aktualisieren
        p.x += v.x * globalMilliTicksSinceLastDraw;
        p.y += v.y * globalMilliTicksSinceLastDraw;

        // Zeichnen
        glColor4f(vars.col[0], vars.col[1], vars.col[2], (1.0f / static_cast<float>(life)) * static_cast<float>(lifeleft));

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
    } else {
        active = false;
    }
}

// --- EffectsTransist Implementierung ---

EffectsTransist::EffectsTransist() {
    opacity = 0.0f;
    age = 0;
}

void EffectsTransist::init() {
    age = 0;
    opacity = 0.0f;
    vars.transition_half_done = 0;
}

void EffectsTransist::draw() {
    age += globalTicksSinceLastDraw;

    if (age < vars.life / 2.0f) { // erste Hälfte
        // Aufblenden
        opacity += vars.life / 500.0f * globalMilliTicksSinceLastDraw;
    } else {
        vars.transition_half_done = 1;
        // Ausblenden
        opacity -= vars.life / 500.0f * globalMilliTicksSinceLastDraw;
    }

    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    glColor4f(vars.col[0], vars.col[1], vars.col[2], opacity);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0);
    glVertex3f(1.0f, 1.0f, 0.0);
    glVertex3f(1.0f, -1.0f, 0.0);
    glVertex3f(-1.0f, -1.0f, 0.0);
    glEnd();
}

// --- particleFieldClass Implementierung ---

void particleFieldClass::init(effect_vars varsP, position spawnPos) {
    vars = varsP;
    spawnTimeout = 0;
    vars.active = true;
    sparks = new sparkle[vars.num];
    p = spawnPos;

    for (int i = 0; i < vars.num; i++) {
        sparks[i].active = true;
        sparks[i].vars = vars;
        spawnSpark(i);
    }
}

particleFieldClass::~particleFieldClass() {
    delete[] sparks; // Speicher freigeben
}

void particleFieldClass::spawnSpark(int sparkNum) {
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

void particleFieldClass::draw() {
    spawnTimeout -= globalTicksSinceLastDraw;

    int t = 0;

    for (int i = 0; i < vars.num; i++) {
        if (sparks[i].active) {
            t++;
            sparks[i].draw();
        }
    }

    vars.life -= globalTicksSinceLastDraw;
    if (t == 0) {
        vars.active = 0;
    }
}

void particleFieldClass::move(position newPos) {
    p = newPos;
}

// --- effect_class Implementierung ---

effect_class::effect_class() {
    vars.active = false; // Nicht aktiv bei Erstellung
    pf = nullptr;
    sparks = nullptr;
}

void effect_class::init(position p) {
    vars.active = true;
    spawn_pos = p;

    switch (vars.type) {
        case FX_SPARKS:
            sparks = new sparkle[vars.num];
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
            pf = new particleFieldClass;
            pf->init(vars, p);
            break;
        default:
            break;
    }
}

void effect_class::draw() {
    bool stay = false;
    
    switch (vars.type) {
        case FX_SPARKS:
            for (int i = 0; i < vars.num; i++) {
                if (sparks[i].active) {
                    sparks[i].draw();
                    stay = true;
                }
            }
            break;

        case FX_TRANSIT:
            transit.draw();
            if (transit.age <= vars.life)
                stay = true;
            break;

        case FX_PARTICLEFIELD:
            pf->draw();
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

EffectManager::EffectManager(EventManager* eventMgr) : eventManager(eventMgr) {
    effects.clear();
    effectId = 0;
    
    // Event-Listener registrieren
    registerEventListeners();
}

EffectManager::~EffectManager() {
    effects.clear();
}

void EffectManager::registerEventListeners() {
    // Event-Handler für Kollisionen registrieren
    eventManager->addListener(GameEvent::BallHitPaddle, 
        [this](const EventData& data) { this->handleBallPaddleCollision(data); });
    
    eventManager->addListener(GameEvent::BrickDestroyed,
        [this](const EventData& data) { this->handleBrickDestroyed(data); });
    
    eventManager->addListener(GameEvent::PowerUpCollected,
        [this](const EventData& data) { this->handlePowerUpCollected(data); });
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

void EffectManager::draw() {
    for (auto it = effects.begin(); it != effects.end();) {
        it->draw();

        if (!it->vars.active) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
}

int EffectManager::isActive(int id) {
    for (const auto& effect : effects) {
        if (effect.vars.effectId == id && effect.vars.active) {
            return 1;
        }
    }
    return 0;
}

void EffectManager::kill(int id) {
    for (auto& effect : effects) {
        if (effect.vars.effectId == id) {
            effect.vars.active = false;
        }
    }
}

// Event-Handler-Implementierungen

void EffectManager::handleBallPaddleCollision(const EventData& data) {
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

void EffectManager::handleBallBrickCollision(const EventData& data) {
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

void EffectManager::handleBrickDestroyed(const EventData& data) {
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

void EffectManager::handlePowerUpCollected(const EventData& data) {
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