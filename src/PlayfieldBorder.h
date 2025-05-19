#pragma once

#include "GameObject.h"
#include "SpriteSheetAnimation.h"


class PlayfieldBorder final : public GameObject {
public:
    enum class Side { Left, Right };

    PlayfieldBorder(Side side, const SpriteSheetAnimation &tex);

    void init() override;

    void update(float) override {
    }

    void draw(float deltaTime) override;

private:
    Side side;
    GLuint dl = 0;
};
