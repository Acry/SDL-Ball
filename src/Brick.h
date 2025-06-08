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
    } // Leere Implementation
    void update(float deltaTime) override {
    } // Leere Implementation
    void draw(float deltaTime) override;

    ~Brick() override = default;
};
