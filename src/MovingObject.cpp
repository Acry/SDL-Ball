// MovingObject.cpp
#include "MovingObject.h"

void MovingObject::update(const float deltaTime) {
    pos_x += xvel * deltaTime;
    pos_y += yvel * deltaTime;
}
