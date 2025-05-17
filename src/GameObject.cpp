#include "GameObject.h"

bool GameObject::isCollidingWith(const GameObject& other) const {
    // Einfache AABB-Kollisionserkennung (Axis-Aligned Bounding Box)
    return (pos_x - width < other.pos_x + other.width &&
            pos_x + width > other.pos_x - other.width &&
            pos_y - height < other.pos_y + other.height &&
            pos_y + height > other.pos_y - other.height);
}