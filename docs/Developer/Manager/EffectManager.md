# EffectManager Class

The `EffectManager` class is responsible for creating, managing, and rendering visual effects in the SDL-Ball project.
It provides a flexible interface to spawn and control different types of effects that enhance the visual feedback and
polish of the game.

## Supported Effects

- **Spark Effects (FX_SPARKS):**
  Creates a burst of small particles (sparks) that simulate collisions, explosions, or impacts. Each spark is animated
  with its own velocity, gravity, color, and lifetime, fading out as it moves.
  Funkeneffekte (FX_SPARKS): Erzeugt mehrere kleine Partikel, die sich in verschiedene Richtungen bewegen und mit der
  Zeit verblassen.

Funkeneffekte (FX_SPARKS)
Beschreibung: Erzeugt mehrere kleine Partikel, die sich in verschiedene Richtungen bewegen und verblassen
Anwendung: Kollisionen, Explosionen, Aufpralleffekte
Eigenschaften: Bewegung mit Schwerkraft, Abprallen, individuelle Lebenszeit

- **Fade/Transition Effects (FX_TRANSIT):**
  Draws a full-screen colored quad that fades in and out, used for scene transitions or to highlight important game
  events. The fade effect smoothly animates the opacity over a configurable duration.
  Übergangseffekte (FX_TRANSIT): Ein Fade-In/Fade-Out-Effekt mit einer halbtransparenten Farbfläche, die langsam
  erscheint und dann wieder verschwindet.

Übergangseffekte (FX_TRANSIT)
Beschreibung: Fade-In/Fade-Out mit halbtransparenter Farbfläche
Anwendung: Szenenwechsel, wichtige Spielereignisse
Eigenschaften: Animierte Transparenz, einstellbare Dauer

- **Particle Field Effects (FX_PARTICLEFIELD):**
  Emits a field of particles from a given position, often used for power-up collection, special effects, or area-based
  visual feedback. Each particle behaves similarly to a spark, but the effect can be shaped and colored as needed.
  Partikelfeld (FX_PARTICLEFIELD): Erzeugt kontinuierlich neue Partikel in einem bestimmten Bereich mit eigener
  Kollisionserkennung.

Partikelfelder (FX_PARTICLEFIELD)
Beschreibung: Kontinuierliche Partikelemission in einem bestimmten Bereich
Anwendung: Power-Up-Effekte, Spezialeffekte, flächige visuelle Rückmeldungen
Eigenschaften: Form- und Farbkonfiguration, mehrere Partikel gleichzeitig

Die Effekte haben folgende gemeinsame Eigenschaften:

Farben
Lebensdauer
Position
Größe
Geschwindigkeit
Kollisionserkennung (optional)
Gravitation (für Partikel)

Die Partikel können mit Blöcken (brick) und dem Paddel (paddle_class) kollidieren und darauf reagieren.

Gemeinsame Eigenschaften aller Effekte
Eigenschaft
Beschreibung
Farbe
RGB-Farbwerte für die visuelle Darstellung
Lebensdauer
Wie lange der Effekt aktiv bleibt (in ms)
Position
Ort im Spielfeld, wo der Effekt erscheint
Größe
Skalierungsfaktor der Effektelemente
Geschwindigkeit
Bewegungsrate der Partikel
Kollisionserkennung
Optional: Reagieren auf Spielobjekte
Schwerkraft
Beeinflusst Partikelbewegung (nach unten)

Event-Integration
Der EffectManager ist mit dem EventManager verknüpft und reagiert automatisch auf:
Ball-Paddle-Kollisionen (BallHitPaddle)
Brick-Zerstörungen (BrickDestroyed)
Power-Up-Sammlungen (PowerUpCollected)
Ball-Brick-Kollisionen (BallHitBrick)
Ball-Lebenszyklus (BallCreated, BallMoved, BallDestroyed)

## How Effects Work

- Each effect type is implemented as a class (`Sparkle`, `Fade`, `Particles`) with its own properties and update/draw
  logic.
- Effects are parameterized by properties such as number of particles, color, size, speed, gravity, texture, and
  lifetime. These can be set via the `EffectManager::set` methods before spawning an effect.
- Effects are spawned at a specific position using `EffectManager::spawn(position)`. The manager keeps track of all
  active effects and updates/draws them each frame.
- Effects are automatically removed when they finish (e.g., all particles have faded out or the transition is complete).

## Integration with EventManager

The `EffectManager` is tightly integrated with the game's `EventManager`. It registers event listeners for key gameplay
events, such as:

- Ball hitting the paddle
- Brick being destroyed
- Power-up being collected

When these events are emitted by the game logic, the `EffectManager` automatically spawns the appropriate visual effect
at the event's location. This ensures that effects are always in sync with gameplay and provides immediate visual
feedback to the player. Developers can also trigger effects manually if needed.

This event-driven approach makes it easy to extend the game with new effects or to connect additional game events to
visual feedback, simply by adding new event listeners and effect configurations.

## Usage

- To use an effect, configure its parameters using the `set` methods (e.g., set type, color, size, etc.), then call
  `spawn(position)` to create the effect at the desired location.
- The `draw(deltaTime)` method must be called every frame to update and render all active effects. Pass the frame's
  delta time for smooth animation.
- The manager also listens to game events (like collisions or power-up collection) and can automatically trigger
  appropriate effects in response.

## Example

```cpp
// Set up a spark effect
EffectManager effectManager(...);
effectManager.set(FX_VAR_TYPE, FX_SPARKS);
effectManager.set(FX_VAR_NUM, 20);
effectManager.set(FX_VAR_COLOR, 1.0f, 0.5f, 0.2f);
effectManager.spawn({x, y});

// In the game loop:
effectManager.draw(deltaTime);
```

## Summary

The `EffectManager` provides a unified and extensible system for visual effects in the game. It supports multiple effect
types, each with customizable properties, and integrates with the game's event system for automatic effect triggering.
This allows developers to easily add, configure, and manage visual feedback for a more engaging player experience.

## Future Enhancements

Ein möglicher nächster Schritt wäre die Implementierung des FX_FIRE-Effekts, wenn du diesen benötigst, oder die
Erweiterung um weitere Effekttypen wie Explosionen, Wellen oder Lichteffekte.

## Vorschlag

// EffectManager.h
#pragma once

#include <list>
#include <unordered_map>
#include <memory>
#include "MathHelper.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Tracer.h"

#define RAD 6.28318530718 // 2*PI

// Effekttypen
#define FX_SPARKS 0
#define FX_FIRE 1
#define FX_TRANSIT 2
#define FX_PARTICLEFIELD 3
#define FX_TRACER 4

// Effekteigenschaften
#define FX_VAR_TYPE 1
#define FX_VAR_NUM 0
#define FX_VAR_LIFE 2
#define FX_VAR_SPEED 3
#define FX_VAR_SPREAD 4
#define FX_VAR_SIZE 5
#define FX_VAR_GRAVITY 6
#define FX_VAR_COLOR 7
#define FX_VAR_TEXTURE 8
#define FX_VAR_RECTANGLE 9
#define FX_VAR_COLDET 10

// Gemeinsame Struktur für Effekteigenschaften
struct EffectProperties {
int type = 0;
int num = 0;
int life = 0;
float speed = 0.0f;
float spread = 0.0f;
float size = 0.0f;
float gravity = 0.0f;
float color[3] = {1.0f, 1.0f, 1.0f};
TextureResource texture;
position rect = {0.0f, 0.0f};
bool collisionDetection = false;
bool active = false;
int effectId = -1;
int transition_half_done = 0;
};

// Basis-Effektklasse
class BaseEffect {
public:
virtual ~BaseEffect() = default;

    virtual void init(const position& pos) = 0;
    virtual void draw(float deltaTime) = 0;
    virtual bool isActive() const = 0;
    
    int getEffectId() const { return properties.effectId; }

protected:
EffectProperties properties;
};

// Sparkle-Effekt (einzelne Partikel)
class SparkleEffect : public BaseEffect {
public:
SparkleEffect();
~SparkleEffect() override;

    void init(const position& pos) override;
    void draw(float deltaTime) override;
    bool isActive() const override;

private:
class Sparkle {
public:
Sparkle();
void draw(float deltaTime);
bool isActive() const { return active; }

        bool active = true;
        float size = 0.0f;
        float angle = 0.0f;
        int life = 0;
        int lifeLeft = 0;
        position pos, velocity;
        float bounce = 0.0f;
        float friction = 0.0f;
        EffectProperties* parentProps = nullptr;
    };
    
    std::unique_ptr<Sparkle[]> sparkles;
    position spawnPosition;

};

// Übergangseffekt
class TransitionEffect : public BaseEffect {
public:
TransitionEffect();

    void init(const position& pos) override;
    void draw(float deltaTime) override;
    bool isActive() const override;

private:
float opacity = 0.0f;
int age = 0;
};

// Partikelfeld-Effekt
class ParticleFieldEffect : public BaseEffect {
public:
ParticleFieldEffect();
~ParticleFieldEffect() override;

    void init(const position& pos) override;
    void draw(float deltaTime) override;
    bool isActive() const override;
    void move(const position& newPos);

private:
void spawnParticle(int index);

    int spawnTimeout = 0;
    std::unique_ptr<SparkleEffect::Sparkle[]> particles;
    position currentPosition;

};

// EffectManager-Klasse
class EffectManager {
public:
EffectManager(EventManager* eventMgr);
~EffectManager();

    // Konfiguration
    void set(int property, float value);
    void set(int property, int value);
    void set(int property, float r, float g, float b);
    void set(int property, SpriteSheetAnimation texture);
    void set(int property, position rect);
    
    // Effekt-Management
    int spawn(const position& pos);
    void draw(float deltaTime);
    bool isActive(int id) const;
    void kill(int id);
    
    // Tracer-Verwaltung
    int createTracer(float width, float height, bool explosive = false);
    void updateTracer(int tracerId, float x, float y);
    void setTracerColor(int tracerId, bool explosive, const float color[]);
    void setTracerSize(int tracerId, float width, float height);
    void removeTracer(int tracerId);
    void setTracerTexture(int tracerId, SpriteSheetAnimation texture);
    void setTracerActive(int tracerId, bool active);

private:
// Event-Management
void registerEventListeners();
void handleObjectTracerCreate(const EventData& data);
void handleObjectTracerUpdate(const EventData& data);
void handleObjectTracerRemove(const EventData& data);

    // Event-Handler für Spielereignisse
    void handleBallPaddleCollision(const EventData& data);
    void handleBrickDestroyed(const EventData& data);
    void handlePowerUpCollected(const EventData& data);
    void handleBallBrickCollision(const EventData& data);
    
    // Effekt-Verwaltung
    std::list<std::unique_ptr<BaseEffect>> effects;
    EffectProperties currentProperties;
    int nextEffectId = 0;
    
    // Tracer-Verwaltung
    std::unordered_map<int, std::unique_ptr<Tracer>> tracers;
    std::unordered_map<int, int> objectTracers;
    
    EventManager* eventManager = nullptr;

};