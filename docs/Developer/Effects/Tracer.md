# Tracer

# Auslagern des Tracers als eigenständige Komponente

Ähnlich wie beim TransitionEffect solltest du für den Tracer eine eigenständige Komponentenarchitektur erstellen, aber
mit speziellen Anpassungen für die Ball-Lebenszyklus-Bindung und ICollidable-Funktionalität.

## Schritt 1: Definiere TracerProperties in EffectDefinitions.h

```cpp
struct TracerProperties : public BaseEffectProperties {
    int maxPoints = 100;
    float fadeRate = 0.05f;     // Wie schnell der Tracer verblasst
    bool rainbowEffect = false;
    float thickness = 0.02f;    // Dicke des Tracers
    Color tailColor = {1.0f, 0.0f, 0.0f, 1.0f};
    ObjectId targetObjectId = -1; // ID des verfolgten Objekts
    bool isCollidable = false;  // Ob der Tracer Kollisionen auslösen kann
};
```

## Schritt 2: Tracer als eigenständige Klasse überarbeiten

```cpp
// Tracer.h
#pragma once
#include "BaseEffect.h"
#include "ICollideable.h"
#include "EffectDefinitions.h"

class Tracer : public BaseEffect, public ICollideable {
public:
    explicit Tracer(const TextureResource &tex);
    ~Tracer() override = default;

    // BaseEffect Methoden
    void init(const position &pos) override;
    void update(float deltaTime) override;
    void draw() const override;
    [[nodiscard]] bool isActive() const override;

    // Tracer-spezifische Methoden
    void enableRainbow(bool enable);
    [[nodiscard]] bool isRainbowEnabled() const;
    void setSize(float factor);
    void colorRotate(bool explosive, const GLfloat c[]);
    void updatePosition(float nx, float ny);
    void setLastPosition(float nx, float ny);

    // ICollideable Interface
    void setActive(bool value) override;
    [[nodiscard]] float getPosX() const override;
    [[nodiscard]] float getPosY() const override;
    [[nodiscard]] float getWidth() const override;
    [[nodiscard]] float getHeight() const override;
    [[nodiscard]] bool isCollisionActive() const override;
    [[nodiscard]] std::vector<float> *getCollisionPoints() const override;
    [[nodiscard]] CollisionType getCollisionType() const override;

private:
    TracerProperties properties;
    
    GLfloat x[100], y[100]; // Position
    GLfloat r[100], g[100], b[100]; // Farbe
    GLfloat a[100]; // Alpha
    GLfloat s[100]; // Größe
    GLfloat cr{1.0f}, cg{0.0f}, cb{0.0f}; // Aktuelle Farben
    bool active[100];
    int color{0};
    GLfloat lastX{0.0f}, lastY{0.0f}; // Letzte Position
    
    void rainbowColorUpdate(float deltaTime);
    mutable std::vector<float> collisionPoints;
    void updateCollisionPoints() const;
};
```

## Schritt 3: TracerPresets in EffectPresets.h ergänzen

```cpp
inline TracerProperties createBallTracerPreset() {
    TracerProperties props;
    
    // BaseEffectProperties Felder
    props.category = EffectCategory::Tracer;
    props.life = 0; // Unbegrenzte Lebensdauer, wird mit Ball-Objekt synchronisiert
    props.size = 0.02f;
    props.color = {0.7f, 0.7f, 1.0f, 1.0f}; // Bläulich
    props.active = true;
    
    // TracerProperties Felder
    props.maxPoints = 100;
    props.fadeRate = 0.05f;
    props.rainbowEffect = false;
    props.thickness = 0.02f;
    props.isCollidable = false;
    
    return props;
}

inline TracerProperties createExplosiveTracerPreset() {
    TracerProperties props = createBallTracerPreset();
    props.color = {1.0f, 0.5f, 0.0f, 1.0f}; // Orange-Rot für explosive Tracer
    props.fadeRate = 0.08f; // Schnelleres Verblassen
    return props;
}

inline TracerProperties createCollidableTracerPreset() {
    TracerProperties props = createBallTracerPreset();
    props.isCollidable = true;
    props.thickness = 0.03f; // Etwas dicker für bessere Kollisionserkennung
    return props;
}
```

## Schritt 4: Methode in EffectFactory hinzufügen

```cpp
std::unique_ptr<Tracer> createBallTracer(int ballId, bool explosive = false) {
    TextureResource tracerTexture = m_textureManager->getEffectTexture(EffectTexture::Tail)->textureProperties;
    auto tracer = std::make_unique<Tracer>(tracerTexture);
    
    if (explosive) {
        tracer->setProperties(EffectPresets::createExplosiveTracerPreset());
    } else {
        tracer->setProperties(EffectPresets::createBallTracerPreset());
    }
    
    tracer->setTargetObjectId(ballId);
    tracer->init({0.0f, 0.0f});
    return tracer;
}
```

## Schritt 5: TracerManager für Ball-Tracer-Zuordnungen

```cpp
// TracerManager.h
class TracerManager {
public:
    explicit TracerManager(EventManager* eventManager, EffectFactory* factory);
    ~TracerManager();
    
    void update(float deltaTime);
    void draw() const;
    
private:
    EventManager* m_eventManager;
    EffectFactory* m_effectFactory;
    std::unordered_map<int, std::unique_ptr<Tracer>> tracers;
    
    // Event Handler
    void handleBallCreated(const EventData& data);
    void handleBallMoved(const EventData& data);
    void handleBallDestroyed(const EventData& data);
    void handleBallSizeChanged(const EventData& data);
};
```

## Vorteile dieser Architektur:

1. **Klare Trennung der Zuständigkeiten**:

- Tracer ist eine eigenständige Komponente
- TracerManager übernimmt die Lebenszyklus-Verwaltung
- EventSystem ermöglicht lose Kopplung

2. **Flexibilität**:

- Tracers können sowohl mit Balls als auch unabhängig davon verwendet werden
- Die ICollidable-Schnittstelle ermöglicht Kollisionserkennung

3. **Konsistenz**:

- Folgt dem gleichen Muster wie andere Effekte
- Verwendet Properties- und Factory-Architektur

4. **Bessere Wartbarkeit**:

- Tracers sind vom EffectManager entkoppelt
- Ball-Tracer-Bindung ist explizit

Diese Architektur unterstützt alle deine Anforderungen und folgt dem Pattern, das du bereits für den TransitionEffect
implementiert hast.

## todo

- [ ] TracerManager

- [ ] tracer shows red color and shows up when glued to the paddle, should reset in new level
- [ ] is tracer speed dependent on the speed of the ball?
- [ ] Tracer, if Ball explosive, make Tracer more orange and dont use Colors of bricks bounced to.
- [ ] Tracer refactoring (effectManager/eventManager/inheritance/particleSystem/consistent naming, from LevelTheme to
  instance)
  Named: Tail, Tracer, BallEffect, BallTracer, BallTail ...
  public: tracer tail; - andersrum wäre richtig gewesen. Tracer ist eine Form des Ball-Tails
- [ ] Tracer, inherit from growable

## Bugs

- [ ] [Tracer](GameObjects/Tracer.md) ist shortly visible after ball reset