# GameObject

Brick
Ball
Paddle

LayeredTexture

see: Brick::draw()

can be abstracted to GameObject::draw()
Actually, the brick could be a header-only class, since it has no logic

Maybe the GameObject::draw() method should be renamed to GameObject::drawBase()

Ball:
void drawBase() const;
void drawExplosiveLayer() const;
But actually it is no explosive layer, it is a different texture
The BallManager should handle that.

Paddle:
void drawBase() const;
void drawGlueLayer() const;
void drawGunLayer() const;

GameObject::draw() ->

Welche Methoden der ObjectManager aufruft, ist ihm überlassen
brickManager.draw();

TODO: Blueprint ObjectManager

GrowableObject
MovingObject
