# EffectManager Class

The `EffectManager` class is responsible for creating, managing, and rendering visual effects in the SDL-Ball project. It provides a flexible interface to spawn and control different types of effects that enhance the visual feedback and polish of the game.

## Supported Effects

- **Spark Effects (FX_SPARKS):**
  Creates a burst of small particles (sparks) that simulate collisions, explosions, or impacts. Each spark is animated with its own velocity, gravity, color, and lifetime, fading out as it moves.

- **Fade/Transition Effects (FX_TRANSIT):**
  Draws a full-screen colored quad that fades in and out, used for scene transitions or to highlight important game events. The fade effect smoothly animates the opacity over a configurable duration.

- **Particle Field Effects (FX_PARTICLEFIELD):**
  Emits a field of particles from a given position, often used for power-up collection, special effects, or area-based visual feedback. Each particle behaves similarly to a spark, but the effect can be shaped and colored as needed.

## How Effects Work

- Each effect type is implemented as a class (`Sparkle`, `Fade`, `Particles`) with its own properties and update/draw logic.
- Effects are parameterized by properties such as number of particles, color, size, speed, gravity, texture, and lifetime. These can be set via the `EffectManager::set` methods before spawning an effect.
- Effects are spawned at a specific position using `EffectManager::spawn(position)`. The manager keeps track of all active effects and updates/draws them each frame.
- Effects are automatically removed when they finish (e.g., all particles have faded out or the transition is complete).

## Integration with EventManager

The `EffectManager` is tightly integrated with the game's `EventManager`. It registers event listeners for key gameplay events, such as:

- Ball hitting the paddle
- Brick being destroyed
- Power-up being collected

When these events are emitted by the game logic, the `EffectManager` automatically spawns the appropriate visual effect at the event's location. This ensures that effects are always in sync with gameplay and provides immediate visual feedback to the player. Developers can also trigger effects manually if needed.

This event-driven approach makes it easy to extend the game with new effects or to connect additional game events to visual feedback, simply by adding new event listeners and effect configurations.

## Usage

- To use an effect, configure its parameters using the `set` methods (e.g., set type, color, size, etc.), then call `spawn(position)` to create the effect at the desired location.
- The `draw(deltaTime)` method must be called every frame to update and render all active effects. Pass the frame's delta time for smooth animation.
- The manager also listens to game events (like collisions or power-up collection) and can automatically trigger appropriate effects in response.

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

The `EffectManager` provides a unified and extensible system for visual effects in the game. It supports multiple effect types, each with customizable properties, and integrates with the game's event system for automatic effect triggering. This allows developers to easily add, configure, and manage visual feedback for a more engaging player experience.
