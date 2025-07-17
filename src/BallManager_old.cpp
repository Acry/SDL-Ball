// BallManager.cpp - Angepasst an den Header
#include "BallManager.h"
#include "config.h"
#include "MathHelper.h"

BallManager::BallManager(EventManager* eventMgr) : eventManager(eventMgr) {
    // Auf Ereignisse reagieren
    eventManager->addListener(GameEvent::BallHitBorder,
        [this](const EventData& data) {
            // Reaktion auf Ball-Wand Kollision
            Ball* ball = static_cast<Ball*>(data.sender);
            if (ball && ball->isActive()) {
                // Möglicher visueller Effekt an der Kollisionsposition
            }
        },
        this
    );

    eventManager->addListener(GameEvent::BallHitPaddle,
        [this](const EventData& data) {
            // Reaktion auf Ball-Paddle Kollision
            Ball* ball = static_cast<Ball*>(data.sender);
            if (ball && ball->isActive()) {
                // Visueller Effekt oder Sound-Rückmeldung
            }
        },
        this
    );

    eventManager->addListener(GameEvent::BallLost,
        [this](const EventData& data) {
            // Reaktion auf Ball verloren
            // Diese Logik könnte in den GameManager verschoben werden
            if (getActiveBallCount() == 0) {
                // Spiel verloren oder Leben abziehen
                // Hier nur für Visualisierung
            }
        },
        this
    );
}

BallManager::~BallManager() {
    // Listener entfernen
    eventManager->removeListener(GameEvent::BallHitBorder, this);
    eventManager->removeListener(GameEvent::BallHitPaddle, this);
    eventManager->removeListener(GameEvent::BallLost, this);
    clearAllBalls();
}

bool BallManager::loadTextures(const TextureManager& texManager) {
    // Texturen für normale Bälle, Feuer-Bälle und Tracer laden
    normalBallTexture = texManager.getSpriteSheetAnimation("normalBall");
    fireBallTexture = texManager.getSpriteSheetAnimation("fireBall");
    tracerTexture = texManager.getSpriteSheetAnimation("tracer");

    return normalBallTexture.isValid() && fireBallTexture.isValid() && tracerTexture.isValid();
}

void BallManager::createInitialBall() {
    // Alle vorhandenen Bälle entfernen
    clearAllBalls();

    // Initialen Ball in der Mitte des unteren Bildschirmrands erzeugen
    spawnBall(0.5f, 0.1f, true, 0.0f, 0.3f, 0.0f);
}

void BallManager::clearAllBalls() {
    balls.clear();
}

void BallManager::multiplyActiveBalls() {
    // Zähle aktive Bälle
    size_t activeBallCount = getActiveBallCount();
    if (activeBallCount == 0 || balls.size() >= MAX_BALLS)
        return;

    // Erstelle eine Kopie der aktuellen Bälle
    std::vector<std::unique_ptr<Ball>> currentBalls;
    currentBalls.reserve(balls.size());

    // Bewege alle aktuellen Bälle in eine temporäre Liste
    for (auto& ball : balls) {
        if (ball->isActive()) {
            currentBalls.push_back(std::move(ball));
        }
    }

    // Entferne alle Bälle aus dem Hauptvektor
    balls.clear();
    balls.reserve(std::min(MAX_BALLS, currentBalls.size() * 2));

    // Füge die vorhandenen Bälle wieder hinzu
    for (auto& ball : currentBalls) {
        balls.push_back(std::move(ball));
    }

    // Erzeuge Klone der aktiven Bälle
    for (size_t i = 0; i < currentBalls.size() && balls.size() < MAX_BALLS; ++i) {
        Ball* original = balls[i].get();

        // Erzeuge einen neuen Ball mit ähnlichen Eigenschaften
        spawnBall(
            original->pos_x,
            original->pos_y,
            false,
            0.0f,
            original->velocity,
            // Leicht abweichender Winkel für den neuen Ball
            original->getAngle() + MathHelper::randomFloat(-15.0f, 15.0f)
        );

        // Übernimm die Eigenschaften wie explosive, Größe usw.
        Ball* clone = balls.back().get();
        clone->explosive = original->explosive;

        // Setze Größe entsprechend dem Original
        if (original->width < 0.02f) { // Kleiner Ball
            clone->setSize(0.015f);
        } else if (original->width > 0.03f) { // Großer Ball
            clone->setSize(0.04f);
        }
    }
}

void BallManager::releaseGluedBalls() {
    for (auto& ball : balls) {
        if (ball->isActive() && ball->glued) {
            ball->glued = false;
            ball->launchFromPaddle();
        }
    }
}

void BallManager::spawnBall(float posX, float posY, bool glued, float gluedX, float speed, float angle) {
    // Prüfe, ob maximale Anzahl Bälle erreicht ist
    if (balls.size() >= MAX_BALLS)
        return;

    // Neuen Ball erstellen
    auto newBall = std::make_unique<Ball>(eventManager);

    // Ball-Eigenschaften setzen
    newBall->texture = normalBallTexture;
    newBall->fireTex = fireBallTexture;
    newBall->tracer.tex = &tracerTexture;

    newBall->pos_x = posX;
    newBall->pos_y = posY;
    newBall->glued = glued;
    newBall->gluedX = gluedX;
    newBall->setActive(true);
    newBall->setSpeed(speed);
    newBall->setAngle(angle);
    newBall->setSize(0.025f); // Standard-Größe

    balls.push_back(std::move(newBall));
}

void BallManager::update(float deltaTime, const Paddle& paddle) {
    // Update alle aktiven Bälle
    for (auto it = balls.begin(); it != balls.end();) {
        auto& ball = *it;

        if (ball->isActive()) {
            // Wenn der Ball am Paddle klebt, Position aktualisieren
            if (ball->glued) {
                ball->pos_x = paddle.pos_x + ball->gluedX;
                ball->pos_y = paddle.pos_y + paddle.height + ball->height;
            }

            // Ball-Logik ausführen
            ball->update(deltaTime);
            ++it;
        } else {
            // Inaktive Bälle entfernen
            it = balls.erase(it);
        }
    }
}

void BallManager::draw(float deltaTime) {
    for (auto& ball : balls) {
        if (ball->isActive()) {
            ball->draw(deltaTime);
        }
    }
}

void BallManager::applyPowerup(int powerupType) {
    for (auto& ball : balls) {
        if (ball->isActive()) {
            switch (powerupType) {
                case 1: // Großer Ball (PO_BIGBALL)
                    ball->setSize(0.04f);
                    ball->setSpeed(0.3f); // Standard-Geschwindigkeit
                    break;
                case 2: // Kleiner Ball (PO_SMALLBALL)
                    ball->setSize(0.015f);
                    // Schnellere Geschwindigkeit für kleine Bälle
                    ball->setSpeed(ball->velocity * 1.2f);
                    break;
                case 3: // Normaler Ball (PO_NORMALBALL)
                    ball->setSize(0.025f);
                    ball->setSpeed(0.3f);
                    break;
                case 4: // Explosiver Ball (PO_EXPLOSIVE)
                    ball->explosive = true;
                    ball->tracer.colorRotate(true, nullptr);
                    break;
                // Weitere Powerup-Typen können hier hinzugefügt werden
            }
        }
    }
}

size_t BallManager::getActiveBallCount() const {
    size_t count = 0;
    for (const auto& ball : balls) {
        if (ball->isActive()) {
            count++;
        }
    }
    return count;
}

float BallManager::calculateAverageBallSpeed() const {
    float totalSpeed = 0.0f;
    size_t activeBalls = 0;

    for (const auto& ball : balls) {
        if (ball->isActive()) {
            totalSpeed += ball->velocity;
            activeBalls++;
        }
    }

    return (activeBalls > 0) ? totalSpeed / static_cast<float>(activeBalls) : 0.0f;
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

std::vector<Ball*> BallManager::getBalls() {
    std::vector<Ball*> ballPtrs;
    ballPtrs.reserve(balls.size());

    for (auto& ball : balls) {
        ballPtrs.push_back(ball.get());
    }

    return ballPtrs;
}
