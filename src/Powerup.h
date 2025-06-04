// Powerup.h
#pragma once
#include "MovingObject.h"
#include "SpriteSheetAnimation.h"
#include "EventManager.h"

enum class PowerUpType {
    None,
    GrowPaddle,      // '1'
    ShrinkPaddle,    // '2'
    Die,             // '3'
    Glue,            // '4'
    Multiball,       // '5'
    GoThrough,       // '6'
    Drop,            // '7'
    Detonate,        // '8'
    ExplosiveGrow,   // '9'
    EasyBrick,       // 'A'
    Explosive,       // 'B'
    NextLevel,       // 'C'
    AimHelp,         // 'D'
    Coin,            // 'E'
    BigBall,         // 'F'
    NormalBall,      // 'G'
    SmallBall,       // 'H'
    Aim,             // 'I'
    Life,            // 'O'
    Gun,             // 'P'
    Laser,           // 'R'

    // Spezielle Random-Powerups
    Random10,        // 'J' - 10% Chance
    Random100,       // 'K' - 100% Chance
    Random5,         // 'L' - 5% Chance
    Random2,         // 'M' - 2% Chance
    Random1,         // 'N' - 1% Chance
    RandomEvil       // 'Q' - 100% böses PowerUp
};

class Powerup final : public MovingObject {
    PowerUpType type;
    bool active;
    EventManager* eventManager;
    SpriteSheetAnimation texture;

    static constexpr float GRAVITY = 0.7f;
    static constexpr float DEFAULT_WIDTH = 0.055f / 1.25f;
    static constexpr float DEFAULT_HEIGHT = 0.055f / 1.25f;

public:
    explicit Powerup(EventManager* eventMgr);
    ~Powerup() = default;

    void init() override;
    void update(float deltaTime) override;
    void draw(float deltaTime) override;

    void activate(PowerUpType powerupType, float posX, float posY);
    void deactivate();

    [[nodiscard]] bool isActive() const { return active; }
    [[nodiscard]] PowerUpType getType() const { return type; }
    [[nodiscard]] int getPoints() const;
};
