# Timing

frameAge:
Misst wie "alt" der aktuelle Frame ist in Millisekunden
Wird nach jedem Draw auf 0 zurückgesetzt
Wird verwendet um die FPS-Begrenzung zu steuern (maxFrameAge)

Globale Timer:
globalTicks = SDL_GetTicks() - lastTick;        // Vergangene Zeit seit letztem Tick in ms
globalMilliTicks = globalTicks / 1000.0;        // Vergangene Zeit in Sekunden

globalTicksSinceLastDraw = 0;        // Zeit seit letztem Frame-Draw in ms 
globalMilliTicksSinceLastDraw = 0;   // Zeit seit letztem Frame-Draw in Sekunden

maxFrameAge = (1000 / setting.fps);  // Maximale Frame-Zeit in ms
// Beispiel: Bei 60 FPS = 1000/60 ≈ 16.67ms pro Frame

Pause-Handling:
Bei Pause werden globalTicks und globalMilliTicks auf 0 gesetzt
nonpausingLastTick läuft weiter, auch während Pause
Der Timer wird hauptsächlich verwendet für:

FPS-Begrenzung
Bewegungsberechnung
Animation-Timing
Spielphysik


___
        // timing
        nonpausingGlobalTicks = SDL_GetTicks() - nonpausingLastTick;
        frameAge += nonpausingGlobalTicks;

        nonpausingGlobalMilliTicks = nonpausingGlobalTicks / 1000.0f;
        nonpausingLastTick = SDL_GetTicks();

        if (!var.paused) {
            globalTicks = SDL_GetTicks() - lastTick;
            globalMilliTicks = globalTicks / 1000.0;
        } else {
            globalTicks = globalMilliTicks = 0;
        }
        lastTick = SDL_GetTicks();
# IST
in main wird
```c++

announce.draw();
SDL_GL_SwapWindow(display.sdlWindow);
frameAge = 0;
globalTicksSinceLastDraw = 0;
globalMilliTicksSinceLastDraw = 0;
```
wird nur ausgeführt, wenn:
//really ugly... but easy
`if (!var.titleScreenShow)`

und in title am Ende der draw funktion:

```c++
SDL_GL_SwapWindow(display.sdlWindow );
globalTicksSinceLastDraw=0;
globalMilliTicksSinceLastDraw=0;
*frameAge = 0;
```

in main am Ende der Loop ein statisches usleep(1000);

___

# Option 1 - Fixed Timestep

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