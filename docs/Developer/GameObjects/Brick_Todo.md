# Brick_Todo.md

## BrickTests

brick needs to rendered, after lost hp (isDestroying)
visible, active, destroyed, hit
Bricks destroyed, active, visible, Fade or shrink?
[[nodiscard]] bool isDestroying() const { return state == BrickState::Destroying; }
[[nodiscard]] bool isCollisionEnabled() const { return state == BrickState::Active; }
Render all Bricks
InvisibleBricks rot umranden.
Click a Brick to hit

## Future

I need a new Brick Class, which is indestructible, but can be hit by the Ball.
Does not count on level clear.
destructible Bricks, indestructible Bricks
