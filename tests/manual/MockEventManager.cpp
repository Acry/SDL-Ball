// MockEventManager.cpp
#include "MockEventManager.h"
#include "ICollideable.h"
#include <SDL.h>


void MockEventManager::emit(GameEvent event, const EventData &data) {
    eventCount++;
    lastEvent = event;
    const char *eventNames[] = {
        "BallHitLeftBorder", // Index 0
        "BallHitRightBorder", // Index 1
        "BallHitTopBorder", // Index 2
        "BallLost", // Index 3
        "BallHitPaddle", // Index 4
        "BrickDestroyed", // Index 5
        "PowerUpCollected", // Index 6
        "BallHitBrick", // Index 7
        "PaddleHitLeftBorder", // Index 8
        "PaddleHitRightBorder", // Index 9
        "BallCreated", // Index 10
        "BallMoved", // Index 11
        "BallDestroyed" // Index 12
    };

    // Überprüfe, ob der Index für das Event gültig ist
    if (const int eventIndex = static_cast<int>(event);
        eventIndex >= 0 && static_cast<size_t>(eventIndex) < std::size(eventNames)) {
        SDL_Log("Event ausgelöst: %s (#%d) an Position (%.2f, %.2f)",
                eventNames[eventIndex], eventCount,
                data.posX, data.posY);
    } else {
        SDL_Log("Unbekanntes Event (#%d) ausgelöst", eventCount);
    }

    if (data.sender && data.target) {
        // Target ist immer ein ICollideable
        auto *target = static_cast<ICollideable *>(data.target);
        int targetType = target->getCollisionType();

        // Prüfe, welches Event ausgelöst wurde und handle den Sender entsprechend
        int senderType;

        // Bei Border-Events ist der Sender ein PlayfieldBorder
        if (event == GameEvent::BallHitLeftBorder ||
            event == GameEvent::BallHitRightBorder ||
            event == GameEvent::BallHitTopBorder ||
            event == GameEvent::PaddleHitLeftBorder ||
            event == GameEvent::PaddleHitRightBorder) {
            // Extrahiere den CollidableType direkt aus dem Event
            switch (event) {
                case GameEvent::BallHitLeftBorder:
                case GameEvent::PaddleHitLeftBorder:
                    senderType = static_cast<int>(CollisionType::BorderLeft);
                    break;
                case GameEvent::BallHitRightBorder:
                case GameEvent::PaddleHitRightBorder:
                    senderType = static_cast<int>(CollisionType::BorderRight);
                    break;
                case GameEvent::BallHitTopBorder:
                    senderType = static_cast<int>(CollisionType::BorderTop);
                    break;
                default:
                    senderType = 0; // CollisionType::None
            }
        } else {
            // Für alle anderen Events versuche einen ICollideable-Cast
            auto *sender = static_cast<ICollideable *>(data.sender);
            senderType = sender->getCollisionType();
        }

        const char *collisionTypeNames[] = {
            "None", "Ball", "Paddle", "Brick", "PowerUp",
            "BorderLeft", "BorderRight", "BorderTop"
        };

        if (senderType >= 0 && senderType < 8 && targetType >= 0 && targetType < 8) {
            SDL_Log("Sender: %s, Target: %s",
                    collisionTypeNames[senderType],
                    collisionTypeNames[targetType]);
        } else {
            SDL_Log("Ungültiger Kollisionstyp erkannt: Sender=%d, Target=%d",
                    senderType, targetType);
        }
    }
}
