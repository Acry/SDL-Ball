#include <iostream>

#include "Display.hpp"
#include <memory>
#include "colors.h"

using namespace std;

Display::Display(const int display, const int width, const int height, const bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Fehler bei SDL Video Init: %s", SDL_GetError());
        return;
    }
    // Versuche das Display aus den Settings.
    numOfDisplays = SDL_GetNumVideoDisplays();
    if (numOfDisplays > 0 && display < numOfDisplays) {
        displayToUse = display;
    } else {
        displayToUse = 0;
    }
    unsigned int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (fullscreen) {
        // Im Vollbildmodus native Auflösung verwenden.
        SDL_DisplayMode currentDisplayMode;
        SDL_GetCurrentDisplayMode(displayToUse, &currentDisplayMode);
        currentW = currentDisplayMode.w;
        currentH = currentDisplayMode.h;
        flags |= SDL_WINDOW_FULLSCREEN;
    } else {
        // Im Fenstermodus konfigurierte Auflösung verwenden.
        currentW = width > 0 ? width : 1280; // Fallback-Wert
        currentH = height > 0 ? height : 720; // Fallback-Wert
    }

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
        return;
    }
    if (!initOpenGL(flags)) {
        SDL_Log("Error:%s", SDL_GetError());
        return;
    }
    resize(currentW, currentH);
}

bool Display::updateForMenu() {
    return true;
}

void Display::resize(const int width, const int height) {
    if (height == 0)
        return;

    currentW = width;
    currentH = height;

    window_ratio = static_cast<GLfloat>(currentW) / static_cast<GLfloat>(currentH);

    // Ziel-Seitenverhältnis 4:3
    const float target_ratio = 4.0f / 3.0f;
    int vpW, vpH;

    if (window_ratio >= target_ratio) {
        // Fenster ist breiter als 4:3 -> Höhe begrenzt
        vpH = height;
        vpW = height * target_ratio;
    } else {
        // Fenster ist schmaler als 4:3 -> Breite begrenzt
        vpW = width;
        vpH = width / target_ratio;
    }

    viewportX = (width - vpW) / 2;
    viewportY = (height - vpH) / 2;
    viewportH = vpH;
    viewportW = vpW;
    playfield_ratio = 4.0f / 3.0f;
    glunits_per_xpixel = 2.0f / static_cast<GLfloat>(viewportW);
    glunits_per_ypixel = 2.0f / static_cast<GLfloat>(viewportH);

    glViewport(viewportX, viewportY, viewportW, viewportH);
    glScissor(viewportX, viewportY, viewportW, viewportH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Von -1 bis +1 in NDC, direkt Stein an Stein
    // glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2
    //         +1
    //         ^
    //         |
    // -1 <----+----> +1
    //         |
    //        -1
    glOrtho(-1, 1, -1, 1, -1, 1); // NDC projection, flipping bottom and top for SDL2

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool Display::screenshot(const filesystem::path &pathName) const {
    static constexpr size_t MAX_FILENAME = 256;
    static constexpr size_t TGA_HEADER_SIZE = 12;
    static constexpr size_t TGA_INFO_SIZE = 6;
    static constexpr size_t CHANNELS = 3; // BGR

    char fileName[MAX_FILENAME];
    int fileIndex = 0;

    // Finde freien Dateinamen
    while (fileIndex < 9999) {
        const int result = snprintf(fileName, MAX_FILENAME, "%s/sdl-ball_%04d.tga",
                                    pathName.c_str(), fileIndex);

        if (result < 0 || static_cast<size_t>(result) >= MAX_FILENAME) {
            SDL_Log("Filename too long");
            return false;
        }

        FILE *test = fopen(fileName, "rb");
        if (!test) break;
        fclose(test);
        fileIndex++;
    }

    if (fileIndex == 9999) {
        SDL_Log("No free filename found");
        return false;
    }

    // Alloziere Pixel Buffer
    const size_t pixelCount = currentW * currentH * CHANNELS;
    const auto pixels = std::make_unique<GLubyte[]>(pixelCount);
    if (!pixels) {
        SDL_Log("Memory allocation failed");
        return false;
    }

    // Öffne Ausgabedatei
    FILE *outFile = fopen(fileName, "wb");
    if (!outFile) {
        SDL_Log("Could not create file '%s'", fileName);
        return false;
    }

    // Lese Framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, currentW, currentH, GL_BGR, GL_UNSIGNED_BYTE, pixels.get());

    if (glGetError() != GL_NO_ERROR) {
        SDL_Log("Failed reading OpenGL framebuffer.");
        fclose(outFile);
        return false;
    }

    // Schreibe TGA Header
    const unsigned char tgaHeader[TGA_HEADER_SIZE] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const unsigned char tgaInfo[TGA_INFO_SIZE] = {
        static_cast<unsigned char>(currentW & 0xFF),
        static_cast<unsigned char>(currentW >> 8 & 0xFF),
        static_cast<unsigned char>(currentH & 0xFF),
        static_cast<unsigned char>(currentH >> 8 & 0xFF),
        24, 0
    };

    if (fwrite(tgaHeader, 1, TGA_HEADER_SIZE, outFile) != TGA_HEADER_SIZE ||
        fwrite(tgaInfo, 1, TGA_INFO_SIZE, outFile) != TGA_INFO_SIZE ||
        fwrite(pixels.get(), 1, pixelCount, outFile) != pixelCount) {
        SDL_Log("Failed writing TGA file");
        fclose(outFile);
        return false;
    }

    fclose(outFile);
    return true;
}

bool Display::initOpenGL(const unsigned int flags) {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // Verbesserte Farbtiefe
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Tiefenpuffer-Präzision erhöhen
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    sdlWindow = SDL_CreateWindow("SDL-Ball",
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
                                 currentW, currentH,
                                 flags);
    if (sdlWindow == nullptr) {
        SDL_Log("Error:%s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    glcontext = SDL_GL_CreateContext(sdlWindow);
    if (glcontext == nullptr) {
        SDL_Log("Error:%s", SDL_GetError());
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return false;
    }

    int maj, min;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
    SDL_Log("=== Active OpenGL Context ===");
    SDL_Log("Context Version: %d.%d", maj, min);


    int doubleBuffered = 0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffered);
    SDL_Log("Double buffering %s", doubleBuffered ? "aktiviert" : "deaktiviert");

    /* Enable smooth shading */
    // glShadeModel(GL_SMOOTH);

    /* Set the background black */
    glClearColor(GL_BLACK);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);
    /* The Type Of Depth Test To Do */
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    /* Anisotropes Filtering aktivieren (für bessere Texturqualität) */
    float maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
    if (maxAniso > 0.0f) {
        SDL_Log("Anisotrope Filterung verfügbar: %.1fx", maxAniso);
    }

    glEnable(GL_SCISSOR_TEST);
    return true;
}

Display::~Display() {
    SDL_DestroyWindow(sdlWindow);
    SDL_GL_DeleteContext(glcontext);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_Quit();
}
