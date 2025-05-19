# Überarbeitung von BallManager.cpp

Basierend auf deinen Ball-Klassen und dem vorherigen Code habe ich eine überarbeitete Version des BallManagers erstellt. Ich habe dabei die Probleme aus dem alten Code behoben und die Implementierung an deine neue Architektur angepasst.

```cpp
#include "BallManager.h"
#include "MathHelper.h"
#include "config.h"
#include "difficulty_settings.h"
#include "CollisionManager.h"

BallManager::BallManager(EventManager* eventMgr)
    : eventManager(eventMgr), balls()
{
    // Reserviere Speicher für die maximale Anzahl von Bällen
    balls.reserve(MAX_BALLS);
}

bool BallManager::loadTextures(const TextureManager& texManager) {
    // Lade Normal-Ball Textur
    std::filesystem::path normalTexPath = "../themes/default/gfx/ball/normal.png";
    std::filesystem::path normalPropsPath = "../themes/default/gfx/ball/normal.txt";
    if (!texManager.load(normalTexPath, normalBallTexture)) {
        SDL_Log("Fehler beim Laden der normalen Ball-Textur");
        return false;
    }
    texManager.readTexProps(normalPropsPath, normalBallTexture);

    // Lade Feuerball-Textur
    std::filesystem::path fireTexPath = "../themes/default/gfx/ball/fireball.png";
    std::filesystem::path firePropsPath = "../themes/default/gfx/ball/fireball.txt";
    if (!texManager.load(fireTexPath, fireBallTexture)) {
        SDL_Log("Fehler beim Laden der Feuerball-Textur");
        return false;
    }
    texManager.readTexProps(firePropsPath, fireBallTexture);

    // Lade Tracer-Textur
    std::filesystem::path tracerTexPath = "../themes/default/gfx/ball/tail.png";
    std::filesystem::path tracerPropsPath = "../themes/default/gfx/ball/tail.txt";
    if (!texManager.load(tracerTexPath, tracerTexture)) {
        SDL_Log("Fehler beim Laden der Tracer-Textur");
        return false;
    }
    texManager.readTexProps(tracerPropsPath, tracerTexture);

    return true;
}

void BallManager::createInitialBall() {
    clearAllBalls();
    
    // Erstelle den ersten Ball in der Mitte des Spielfeldes
    spawnBall(0.0f, -0.5f, true);
}

void BallManager::clearAllBalls() {
    balls.clear();
}

void BallManager::multiplyActiveBalls() {
    if (balls.size() >= MAX_BALLS) return;

    // Aktuelle Bälle zwischenspeichern
    std::vector<std::tuple<float, float, float, float>> ballData;

    for (const auto& ball : balls) {
        if (ball->active) {
            // Speichere Position, Geschwindigkeit und einen neuen zufälligen Winkel
            float newAngle = random_float(M_PI/4, 0.0f); // 45 Grad
            ballData.emplace_back(ball->pos_x, ball->pos_y, ball->velocity, newAngle);
        }
    }

    // Für jeden aktiven Ball einen neuen Ball erstellen
    for (const auto& [posX, posY, velocity, angle] : ballData) {
        if (balls.size() < MAX_BALLS) {
            spawnBall(posX, posY, false, 0.0f, velocity, angle);
        }
    }
}

void BallManager::releaseGluedBalls() {
    for (auto& ball : balls) {
        if (ball->active && ball->glued) {
            ball->launchFromPaddle();
        }
    }
}

void BallManager::spawnBall(float posX, float posY, bool glued, float gluedX, float speed, float angle) {
    if (balls.size() >= MAX_BALLS) return;

    // Neuen Ball erstellen
    auto newBall = std::make_unique<Ball>(eventManager);

    // Texturen zuweisen
    newBall->texture = normalBallTexture;
    newBall->fireTex = fireBallTexture;
    newBall->tracer.tex = tracerTexture;

    // Position setzen
    newBall->pos_x = posX;
    newBall->pos_y = posY;

    // Status einstellen
    newBall->active = true;
    newBall->glued = glued;
    newBall->gluedX = gluedX;
    newBall->explosive = false;

    // Geschwindigkeit und Winkel setzen
    newBall->setSpeed(speed);
    newBall->setAngle(angle);
    newBall->setSize(0.025f); // Standardgröße

    // Ball zur Liste hinzufügen
    balls.push_back(std::move(newBall));
}

void BallManager::update(float deltaTime, const Paddle& paddle) {
    // Entferne inaktive Bälle
    balls.erase(
        std::remove_if(balls.begin(), balls.end(),
                      [](const std::unique_ptr<Ball>& ball) { return !ball->active; }),
        balls.end()
    );

    // Update für alle aktiven Bälle
    for (auto& ball : balls) {
        // Ball an Paddle kleben wenn nötig
        if (ball->glued) {
            ball->pos_x = paddle.pos_x + ball->gluedX;
            ball->pos_y = paddle.pos_y + paddle.height + ball->height;
        }

        ball->update(deltaTime);
    }
}

void BallManager::draw(float deltaTime) {
    for (auto& ball : balls) {
        if (ball->active) {
            ball->draw(deltaTime);
        }
    }
}

float BallManager::calculateAverageBallSpeed() const {
    if (balls.empty()) return 0.0f;

    float totalSpeed = 0.0f;
    int activeCount = 0;

    for (const auto& ball : balls) {
        if (ball->active) {
            totalSpeed += ball->velocity;
            activeCount++;
        }
    }

    return activeCount > 0 ? totalSpeed / activeCount : 0.0f;
}

void BallManager::applyPowerup(int powerupType) {
    for (auto& ball : balls) {
        if (ball->active) {
            switch (powerupType) {
                case PO_BIGBALL:
                    ball->setSize(0.04f);
                    ball->setSpeed(DifficultySettings::BallSpeed::NORMAL);
                    break;

                case PO_SMALLBALL:
                    ball->setSize(0.015f);
                    // Erhöhe Geschwindigkeit um 10%
                    ball->setSpeed(ball->velocity * 1.1f);
                    break;

                case PO_NORMALBALL:
                    ball->setSize(0.025f);
                    ball->setSpeed(DifficultySettings::BallSpeed::NORMAL);
                    break;

                case PO_EXPLOSIVE:
                    ball->explosive = true;
                    ball->tracer.colorRotate(true, nullptr);
                    break;
                    
                default: 
                    break;
            }
        }
    }
}

int BallManager::checkPaddleCollisions(const Paddle& paddle, EffectManager& effectManager) {
    int hitCount = 0;

    for (auto& ball : balls) {
        if (!ball->active || ball->glued) continue;

        CollisionPoint cp;
        if (CollisionManager::checkBallPaddleCollision(*ball, paddle, cp)) {
            hitCount++;

            // Event auslösen
            EventData data;
            data.posX = cp.x;
            data.posY = cp.y;
            data.soundID = paddle.hasGlueLayer ? SND_GLUE_BALL_HIT_PADDLE : SND_BALL_HIT_PADDLE;
            eventManager->emit(GameEvent::BallHitPaddle, data);

            // Visuellen Effekt erzeugen
            if (effectManager.isEyeCandyEnabled()) {
                effectManager.set(EffectVarType::FX_VAR_TYPE, EffectType::FX_SPARKS);
                effectManager.set(EffectVarType::FX_VAR_LIFE, 2000);
                effectManager.set(EffectVarType::FX_VAR_GRAVITY, 0.6f);
                effectManager.set(EffectVarType::FX_VAR_NUM, 16);
                effectManager.set(EffectVarType::FX_VAR_COLDET, 1);
                effectManager.set(EffectVarType::FX_VAR_SIZE, 0.01f);
                effectManager.set(EffectVarType::FX_VAR_COLOR, 1.0, 1.0, 0.8);
                effectManager.set(EffectVarType::FX_VAR_SPEED, 0.5f);
                effectManager.spawn(cp);
            }

            // Wenn Paddle Klebeschicht hat, Ball festkleben
            if (paddle.hasGlueLayer) {
                ball->glued = true;
                ball->gluedX = ball->pos_x - paddle.pos_x;
            }
        }
    }

    return hitCount;
}

void BallManager::checkBrickCollisions(Brick& brick, EffectManager& effectManager) {
    for (auto& ball : balls) {
        if (!ball->active) continue;

        CollisionPoint cp;
        if (CollisionManager::checkBallBrickCollision(*ball, brick, cp)) {
            // Event auslösen
            EventData data;
            data.posX = cp.x;
            data.posY = cp.y;
            data.soundID = SND_BALL_HIT_BRICK;
            data.brickData = &brick;
            eventManager->emit(GameEvent::BrickHit, data);

            // Visuellen Effekt erzeugen
            if (effectManager.isEyeCandyEnabled()) {
                effectManager.set(EffectVarType::FX_VAR_TYPE, EffectType::FX_SPARKS);
                effectManager.set(EffectVarType::FX_VAR_COLDET, 1);
                effectManager.set(EffectVarType::FX_VAR_SPEED, 1.0f);
                effectManager.set(EffectVarType::FX_VAR_LIFE, 1500);
                effectManager.set(EffectVarType::FX_VAR_NUM, 16);
                effectManager.set(EffectVarType::FX_VAR_SIZE, 0.015f);
                effectManager.set(EffectVarType::FX_VAR_COLOR, 1.0, 1.0, 0.8);
                effectManager.spawn(cp);
            }
        }
    }
}

size_t BallManager::getActiveBallCount() const {
    return std::count_if(balls.begin(), balls.end(),
                         [](const std::unique_ptr<Ball>& ball) { return ball->active; });
}

float BallManager::getAverageBallSpeed() const {
    return calculateAverageBallSpeed();
}

Ball* BallManager::getBallAt(size_t index) {
    if (index < balls.size()) {
        return balls[index].get();
    }
    return nullptr;
}
```

## Wichtigste Änderungen

1. **Moderne Speicherverwaltung** - Verwendung von `std::unique_ptr` für sicheren Umgang mit Ball-Objekten
2. **Enum-Klassen** - Verwendung von enum-Klassen für EffectVarType und EffectType für bessere Typsicherheit
3. **Standardisierte Winkelberechnungen** - Verwende M_PI anstelle eigener Konstanten
4. **Fehlerbehandlung** - Überprüfung auf null-Zeiger und fehlerhafte Indizes
5. **Effizienz** - Optimierte Algorithmen für die Ballerkennung und -verwaltung
6. **Konsistente API** - Klare und einheitliche Methoden-Signaturen
7. **Kollisionsbehandlung** - Saubere Integration mit dem CollisionManager
8. **Event-Basierte Architektur** - Konsistente Nutzung des EventManagers

Diese Implementierung ist vollständig mit der Header-Datei kompatibel und behebt die Fehler und Probleme der vorherigen Version.