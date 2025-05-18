#include <array>
#include <cstdlib>
#include <unordered_map>

#include "SoundManager.h"
#include "Display.hpp"

int main() {
    Display display(0, 1280, 720, false);
    if (display.sdlWindow == nullptr) {
        SDL_Log("Display konnte nicht initialisiert werden");
        return EXIT_FAILURE;
    }
    SoundManager soundManager;
    // Verfügbare Sound-Themes
    const std::array<std::string, 2> soundThemes = {
        "../themes/default/snd/",
        "../themes/dio-sound-theme/snd/"  // Hier zweites Theme angeben
    };
    int currentTheme = 0;
    soundManager.setSoundTheme(soundThemes[currentTheme]);
    // Zuordnung von Tasten zu Sounds
    std::unordered_map<SDL_Keycode, int> keyToSound = {
        // Zahlenreihe 1-0
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

        // Reihe q-p
        {SDLK_q, SND_PO_HIT_BORDER},
        {SDLK_w, SND_GOOD_PO_HIT_PADDLE},
        {SDLK_e, SND_EVIL_PO_HIT_PADDLE},
        {SDLK_r, SND_SHOT},
        {SDLK_t, SND_DIE},
        {SDLK_z, SND_NEXTLEVEL},  // QWERTZ-Layout
        {SDLK_u, SND_GAMEOVER},
        {SDLK_i, SND_HIGHSCORE},
        {SDLK_o, SND_MENUCLICK},
        {SDLK_p, SND_GLUE_BALL_HIT_PADDLE},

        // Reihe a-g
        {SDLK_a, SND_BUY_POWERUP},
        {SDLK_s, SND_NORM_BRICK_BREAKB},
        {SDLK_d, SND_NORM_BRICK_BREAKC},
        {SDLK_f, SND_NORM_BRICK_BREAKD},
        {SDLK_g, SND_NORM_BRICK_BREAKE}
    };
    bool running = true;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    SDL_Event event;
    while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                display.resize(event.window.data1, event.window.data2);
            }
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            // SPACE für Theme-Wechsel
            if (event.key.keysym.sym == SDLK_SPACE) {
                currentTheme = (currentTheme + 1) % soundThemes.size();
                if (soundManager.setSoundTheme(soundThemes[currentTheme])) {
                    SDL_Log("Sound-Theme gewechselt zu: %s", soundThemes[currentTheme].c_str());
                }
            }
            // Sound abspielen, wenn Taste zugeordnet ist
            auto it = keyToSound.find(event.key.keysym.sym);
            if (it != keyToSound.end()) {
                soundManager.queueSound(it->second, 0.0f);  // Zentrierte Position
                soundManager.play();
                SDL_Log("Sound %d abgespielt", it->second);
            }
        }
    }
    SDL_Delay(13);
    SDL_GL_SwapWindow(display.sdlWindow);
}
    return EXIT_SUCCESS;
}
