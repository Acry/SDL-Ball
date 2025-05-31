// CollisionManager.cpp
#include <cmath>
#include "CollisionManager.h"
#include "Ball.h"
#include "Brick.h"
#include "Paddle.h"
#include "PlayfieldBorder.h"

//        +1
//         ^
//         |
// -1 <----+----> +1
//         |
//        -1
// In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
// (0, 1) ------- (1, 1)
//   |              |
//   |              |
//   |              |
// (0, 0) ------- (1, 0)

CollisionManager::CollisionManager(EventManager *eventMgr) : eventManager(eventMgr) {
    this->eventManager = eventMgr;
}

bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2) {
    //                               LB,               PAD
    const bool horizontalCollision = obj1.getPosX() + obj1.getWidth() > obj2.getPosX() &&
                                     //    RB,               PAD
                                     obj1.getPosX() < obj2.getPosX() + obj2.getWidth();
    // return horizontalCollision;

    //                              B                                   TK
    const bool verticalOverlap = obj1.getPosY() + obj1.getHeight() > obj2.getPosY() &&
                                 //     B                     PAD
                                 obj1.getPosY() < obj2.getPosY() + obj2.getHeight();
    // return verticalOverlap;

    return horizontalCollision && verticalOverlap;
}

bool CollisionManager::checkCollision(const ICollideable &obj1, const ICollideable &obj2,
                                      float &hitX, float &hitY) {
    // Prüfe zuerst die einfache AABB-Kollision
    if (!checkCollision(obj1, obj2)) {
        return false;
    }

    // Bei erfolgreicher Kollision, setze Kollisionspunkt als Mittelpunkt der Überlappung
    float overlapLeft = obj1.getPosX() + obj1.getWidth() - (obj2.getPosX() - obj2.getWidth());
    float overlapRight = obj2.getPosX() + obj2.getWidth() - (obj1.getPosX() - obj1.getWidth());
    float overlapTop = obj1.getPosY() + obj1.getHeight() - (obj2.getPosY() - obj2.getHeight());
    float overlapBottom = obj2.getPosY() + obj2.getHeight() - (obj1.getPosY() - obj1.getHeight());

    // Finde die kleinste Überlappung (die wahrscheinlichste Kollisionsseite)
    float minOverlapX = std::min(overlapLeft, overlapRight);
    float minOverlapY = std::min(overlapTop, overlapBottom);

    if (minOverlapX < minOverlapY) {
        // X-Achsen-Kollision
        hitY = obj1.getPosY();
        if (obj1.getPosX() < obj2.getPosX()) {
            hitX = obj1.getPosX() + obj1.getWidth();
        } else {
            hitX = obj1.getPosX() - obj1.getWidth();
        }
    } else {
        // Y-Achsen-Kollision
        hitX = obj1.getPosX();
        if (obj1.getPosY() < obj2.getPosY()) {
            hitY = obj1.getPosY() + obj1.getHeight();
        } else {
            hitY = obj1.getPosY() - obj1.getHeight();
        }
    }

    return true;
}

void CollisionManager::processCollisions(const std::vector<ICollideable *> &objects) {
    /*
     Im GameManager oder in der Hauptschleife
std::vector<ICollideable*> gameObjects;
gameObjects.push_back(&ball);
gameObjects.push_back(&paddle);
gameObjects.push_back(&leftBorder);
gameObjects.push_back(&rightBorder);
gameObjects.push_back(&topBorder);

// Alle Bricks hinzufügen
for (auto& brick : bricks) {
    gameObjects.push_back(&brick);
}

// Eine einzige Kollisionsprüfung für alles
collisionManager.processCollisions(gameObjects);
    */
    // Alle Objekte miteinander auf Kollisionen prüfen
    const size_t objectCount = objects.size();

    for (size_t i = 0; i < objectCount; ++i) {
        auto *obj1 = objects[i];
        if (!obj1->isActive()) continue;

        for (size_t j = i + 1; j < objectCount; ++j) {
            auto *obj2 = objects[j];
            if (!obj2->isActive()) continue;

            float hitY = 0.0f;
            if (float hitX = 0.0f; checkCollision(*obj1, *obj2, hitX, hitY)) {
                // Kollision erkannt, benachrichtige beide Objekte
                obj1->onCollision(obj2, hitX, hitY);
                obj2->onCollision(obj1, hitX, hitY);
            }
        }
    }
}

void CollisionManager::handleBallBricksCollisions(Ball &ball, std::vector<Brick> &bricks) const {
    for (auto &brick: bricks) {
        if (brick.isActive() && checkCollision(ball, brick)) {
            // Kollisionsrichtung bestimmen
            const float overlapX = std::min(ball.pos_x + ball.width, brick.getPosX() + brick.getWidth()) -
                                   std::max(ball.pos_x, brick.getPosX());
            const float overlapY = std::min(ball.pos_y + ball.height, brick.getPosY() + brick.getHeight()) -
                                   std::max(ball.pos_y, brick.getPosY());

            // Ball-Physik aktualisieren
            if (overlapX < overlapY) {
                ball.xvel = -ball.xvel;
            } else {
                ball.yvel = -ball.yvel;
            }

            // Kollision dem Brick mitteilen
            brick.onCollision(&ball, 0, 0);

            // Event auslösen
            EventData data;
            data.posX = ball.pos_x;
            data.posY = ball.pos_y;
            data.sender = &ball;
            data.target = &brick;
            eventManager->emit(GameEvent::BallHitBrick, data);
            //break;
        }
    }
}

void CollisionManager::handleBallBorderCollisions(Ball &ball,
                                                  const PlayfieldBorder &leftBorder,
                                                  const PlayfieldBorder &rightBorder,
                                                  const PlayfieldBorder &topBorder) const {
    EventData data;
    data.posX = ball.pos_x;
    data.posY = ball.pos_y;
    data.sender = &ball;

    if (checkCollision(ball, rightBorder)) {
        ball.pos_x = rightBorder.getPosX() - ball.width;
        ball.xvel = -ball.xvel;
        data.target = &rightBorder;
        eventManager->emit(GameEvent::BallHitRightBorder, data);
    } else if (checkCollision(ball, leftBorder)) {
        ball.pos_x = leftBorder.getPosX() + leftBorder.getWidth();
        ball.xvel = -ball.xvel;
        data.target = &leftBorder;
        eventManager->emit(GameEvent::BallHitLeftBorder, data);
    }

    if (checkCollision(ball, topBorder)) {
        ball.pos_y = topBorder.getPosY() - ball.height;
        ball.yvel = -ball.yvel;
        data.target = &topBorder;
        eventManager->emit(GameEvent::BallHitTopBorder, data);
    }
}

void CollisionManager::handleBallPaddleCollision(Ball &ball, const Paddle &paddle) const {
    if (checkCollision(ball, paddle)) {
        ball.pos_y = paddle.getPosY() + paddle.height;
        const float relativeIntersectX = (ball.pos_x + ball.width / 2.0f) - (paddle.pos_x + paddle.width / 2.0f);
        const float normalizedIntersect = relativeIntersectX / (paddle.width / 2.0f);
        // Winkelbereich zwischen -60° und 60° (in Radiant)
        const float angle = normalizedIntersect * (M_PI / 3.0f);

        // Geschwindigkeit beibehalten, aber Richtung ändern
        ball.xvel = ball.velocity * sin(angle);
        ball.yvel = ball.velocity * cos(angle);

        // Event auslösen
        EventData data;
        data.posX = ball.pos_x;
        data.posY = ball.pos_y;
        data.sender = &ball;
        data.target = &paddle;
        eventManager->emit(GameEvent::BallHitPaddle, data);
    }
}

void CollisionManager::handlePaddleBorderCollisions(Paddle &paddle,
                                                    const PlayfieldBorder &leftBorder,
                                                    const PlayfieldBorder &rightBorder) const {
    if (checkCollision(leftBorder, paddle)) {
        paddle.pos_x = leftBorder.getPosX() + leftBorder.getWidth();
    } else if (checkCollision(paddle, rightBorder)) {
        paddle.pos_x = rightBorder.getPosX() - paddle.getWidth();
    }
}
