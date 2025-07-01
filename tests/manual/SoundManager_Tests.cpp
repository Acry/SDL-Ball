#include <array>
#include <cstdlib>
#include <unordered_map>

#include "SoundManager.h"
#include "DisplayManager.hpp"
#include "TestHelper.h"
#include "TextManager.h"

// Zuordnung von Tasten zu Sounds
std::unordered_map<SDL_Keycode, int> keyToSound = {
    // 1-0
    {SDLK_1, SND_START},
    {SDLK_2, SND_BALL_HIT_BORDER},
    {SDLK_3, SND_BALL_HIT_PADDLE},
    {SDLK_4, SND_NORM_BRICK_BREAK},
    {SDLK_5, SND_EXPL_BRICK_BREAK},
    {SDLK_6, SND_GLASS_BRICK_HIT},
    {SDLK_7, SND_GLASS_BRICK_BREAK},
    {SDLK_8, SND_INVISIBLE_BRICK_APPEAR},
    {SDLK_9, SND_CEMENT_BRICK_HIT},
    {SDLK_0, SND_DOOM_BRICK_BREAK},

    // q-p
    {SDLK_q, SND_PO_HIT_BORDER},
    {SDLK_w, SND_GOOD_PO_HIT_PADDLE},
    {SDLK_e, SND_EVIL_PO_HIT_PADDLE},
    {SDLK_r, SND_SHOT},
    {SDLK_t, SND_DIE},
    {SDLK_z, SND_NEXTLEVEL}, // QWERTZ-Layout
    {SDLK_u, SND_GAMEOVER},
    {SDLK_i, SND_HIGHSCORE},
    {SDLK_o, SND_MENUCLICK},
    {SDLK_p, SND_GLUE_BALL_HIT_PADDLE},

    // a-g
    {SDLK_a, SND_BUY_POWERUP},
    {SDLK_s, SND_NORM_BRICK_BREAKB},
    {SDLK_d, SND_NORM_BRICK_BREAKC},
    {SDLK_f, SND_NORM_BRICK_BREAKD},
    {SDLK_g, SND_NORM_BRICK_BREAKE}
};

int main() {
    EventManager eventManager;
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Sound Test");
    TextManager textManager(&eventManager);
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
        return EXIT_FAILURE;
    }
    TestHelper testHelper(textManager, &eventManager);
    testHelper.m_showMouseCoords = true;
    SoundManager soundManager;

    const std::array<std::string, 2> soundThemes = {
        "../themes/default",
        "../themes/dio-sound-theme" // Hier zweites Theme angeben
    };
    int currentTheme = 0;
    soundManager.setTheme(soundThemes[currentTheme]);

    const std::vector<std::string> instructions = {
        "Move mouse and play sounds with keys:",
        "1-g: Play sound",
        "Space: Change sound theme",
        "m: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    GLfloat normalizedMouseX = 0.0f, normalizedMouseY = 0.0f;
    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    displayManager.resize(event.window.data1, event.window.data2);
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                normalizedMouseX = (event.motion.x - displayManager.viewportX - displayManager.viewportW / 2.0f) * (
                                       2.0f / displayManager.viewportW);
                normalizedMouseY = (event.motion.y - displayManager.viewportY - displayManager.viewportH / 2.0f) * -1 *
                                   (
                                       2.0f / displayManager.viewportH);
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_m) {
                    testHelper.m_showMouseCoords = !testHelper.m_showMouseCoords;
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    currentTheme = (currentTheme + 1) % soundThemes.size();
                    if (soundManager.setTheme(soundThemes[currentTheme])) {
                        SDL_Log("Sound-Theme gewechselt zu: %s", soundThemes[currentTheme].c_str());
                    }
                }
                if (auto it = keyToSound.find(event.key.keysym.sym); it != keyToSound.end()) {
                    soundManager.queueSound(it->second, normalizedMouseX, normalizedMouseY);
                    soundManager.play();
                    SDL_Log("Sound %d abgespielt", it->second);
                }
            }
        }

        testHelper.updateMousePosition(normalizedMouseX, normalizedMouseY);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        char tempText[64];
        sprintf(tempText, "Current theme: %s", soundThemes[currentTheme].c_str());
        textManager.write(tempText, Fonts::IntroHighscore, true, 1.0f, 0.0f, -0.5f);
        testHelper.drawMouseCoordinates();
        testHelper.renderInstructions(deltaTime, instructions);
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    return EXIT_SUCCESS;
}
