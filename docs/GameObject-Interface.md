# GameObject-Interface

```c++
class GameObject {
public:
    // Basisattribute für alle Spielobjekte
    GLfloat pos_x, pos_y;
    GLfloat width, height;
    GLfloat opacity;
    bool active;
    bool collide;
    bool reflect;
    SpriteSheetAnimation texture;

    // Virtuelle Methoden für Spielobjekt-Lebenszyklus
    virtual ~GameObject() = default;
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;
    
    // Kollisionserkennung
    virtual bool isCollidingWith(const GameObject& other) const;
    
    // Hilfsfunktionen
    bool isActive() const { return active; }
    bool isVisible() const { return opacity > 0.0f; }
};
```