#pragma once

#include "GrowableObject.h"
#include "ICollideable.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"
#include <vector>


class Paddle final : public GrowableObject, public ICollideable {
public:
    explicit Paddle(const texture &tex);

    float centerX{0.0f};
    float centerY{0.0f};
    mutable std::vector<float> collisionPoints;

    TextureResource glueLayerTextureProperties;
    SpriteSheetAnimationProperties glueLayerAnimProps{};

    TextureResource gunLayerTextureProperties;
    SpriteSheetAnimationProperties gunLayerAnimProps{};

    void init() override;

    void draw() const override;

    void update(float deltaTime) override;

    // ICollideable Interface
    void setActive(bool value) override;

    float getPosX() const override;

    float getPosY() const override;

    float getWidth() const override;

    float getHeight() const override;

    bool isActive() const override;

    CollisionType getCollisionType() const override;

    // Paddle-spezifische Methoden
    void setGlueLayer(bool enabled);

    bool getGlueLayer() const;

    void setGunLayer(bool enabled);

    bool getGunLayer() const;

    std::vector<float> *getCollisionPoints() const;

    void onSizeChanged() override;

    bool isPhysicallyActive() const;

    void setPhysicallyActive(bool value);

private:
    void drawBase() const;

    void drawGlueLayer() const;

    void drawGunLayer() const;

    bool collisionActive{true};
    bool active{false};
    bool hasGlueLayer{false};
    bool hasGunLayer{false};
    float aspectRatio{1.0f};
};
