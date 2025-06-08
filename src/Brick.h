// Brick.h
#pragma once

#include "GameObject.h"

class Brick final : public GameObject {
    void drawBase() const;

public:
    bool visible{true};
    Color color{};

    Brick() = default;

    void init() override {
    }

    void update(float deltaTime) override {
    }

    void draw(float deltaTime) override;

    ~Brick() override = default;
};
