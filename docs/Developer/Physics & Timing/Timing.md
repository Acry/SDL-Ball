# Timing

## Option 1 - Fixed Timestep

```c++
void gameLoop() {
    GameTiming timing;
    const float FIXED_TIMESTEP = 1.0f/60.0f;
    float accumulator = 0.0f;

    while (!var.quit) {
        timing.update();
        accumulator += timing.deltaTime;

        // Fixed timestep updates
        while (accumulator >= FIXED_TIMESTEP) {
            updatePhysics(FIXED_TIMESTEP); // Ball/Kollisionen etc
            updateGame(FIXED_TIMESTEP);     // Spiellogik
            accumulator -= FIXED_TIMESTEP;
        }

        // Rendering mit Interpolation
        render(accumulator / FIXED_TIMESTEP);
        
        SDL_GL_SwapWindow(display.sdlWindow);
    }
}
```

## Todo

- [ ] Refactor [Timing](Refactoring/Timing.md)
- [ ] use sdl3 high performance timer, see: [Timing](Physics%20&%20Timing/Timing.md)

## Timescale

If scenes need custom timing (e.g., slow-motion effects), the scene can scale deltaTime internally (e.g.,
effectiveDeltaTime = deltaTime * timeScale).

Threading: If your game uses multiple threads (e.g., rendering vs. logic), ensure deltaTime is synchronized. The code
manager’s single-threaded loop is simplest for now.

