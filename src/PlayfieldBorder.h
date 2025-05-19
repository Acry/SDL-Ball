#pragma once

#include "GameObject.h"
#include "SpriteSheetAnimation.h"

class PlayfieldBorder : public GameObject {
public:
    enum class Side { Left, Right };

    PlayfieldBorder(Side side, const SpriteSheetAnimation& tex, float pillarWidth);
    void init() override {} // keine Initialisierung nötig
    void update(float) override {} // keine Bewegung/Logik
    void draw(float deltaTime) override;

private:
    Side side;
    float pillarWidth;
};

