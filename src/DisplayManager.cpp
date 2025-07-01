// DisplayManager.cpp
#include <iostream>
#include <SDL2/SDL_image.h>

#include "DisplayManager.hpp"
#include "TextureUtilities.h"

DisplayManager::DisplayManager(IEventManager *eventMgr): eventManager(eventMgr), currentW(0), currentH(0),
                                                         numOfDisplays(0),
                                                         glunits_per_xpixel(0),
                                                         glunits_per_ypixel(0),
                                                         viewportX(0),
                                                         viewportY(0), viewportH(0),
                                                         viewportW(0),
                                                         playfield_ratio(0),
                                                         window_ratio(0) {
    eventManager->addListener(GameEvent::WindowResized,
                              [this](const WindowEventData &data) {
                                  this->handleWindowResize(data);
                              }, this);
}

bool DisplayManager::init(const int display, const int width, const int height, const bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Fehler bei SDL Video Init: %s", SDL_GetError());
        return false;
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
        return false;
    }
    if (!initOpenGL(flags)) {
        SDL_Log("Error:%s", SDL_GetError());
        return false;
    }
    resize(currentW, currentH);
    return true;
}

void DisplayManager::handleWindowResize(const WindowEventData &data) {
    resize(data.width, data.height);
}

void DisplayManager::resize(const int width, const int height) {
    if (height == 0)
        return;

    currentW = width;
    currentH = height;

    window_ratio = static_cast<double>(currentW) / static_cast<double>(currentH);

    // Target Aspect-Ratio: 4:3
    constexpr double target_ratio = 4.0f / 3.0f;
    int vpW, vpH;

    if (window_ratio >= target_ratio) {
        // Fenster ist breiter als 4:3 → Höhe begrenzt
        vpH = height;
        vpW = static_cast<int>(height * target_ratio);
    } else {
        // Fenster ist schmaler als 4:3 → Breite begrenzt
        vpW = width;
        vpH = static_cast<int>(width / target_ratio);
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


    // NDC projection: OpenGL
    // Von -1 bis +1 in NDC
    // Die Werte für near und far in glOrtho sind tatsächlich etwas verwirrend.
    // Die Z-Werte -1.0f (near) und 1.0f (far) definieren den sichtbaren Tiefenbereich.
    // -1.0f (near) ist die hintere Clipping-Ebene (weiter vom Betrachter entfernt)
    // und 1.0f (far) ist die vordere Clipping-Ebene (näher am Betrachter).
    // Z = -1.0f wird zu 1.0 im Tiefenpuffer (am weitesten entfernt)
    // Z = 1.0f wird zu 0.0 im Tiefenpuffer (am nächsten zum Betrachter)

    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // left, right, bottom, top, near, far

    // The near and far values used by glOrtho are only distances and (assuming that the eye is located at the origin).
    // Sets up a projection where -nearVal is on the front clipping plane and -farVal is the location of the far
    // clipping plane.

    //        +1
    //         ^
    //         |
    // -1 <----+----> +1
    //         |
    //        -1

    // Flipping the y-axis with glOrtho only affects the projection
    // The viewport’s origin (0, 0) remains at the bottom-left in window
    // coordinates unless you modify the viewport or use a custom framebuffer.

    // In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
    // (0, 1) ------- (1, 1)
    //   |              |
    //   |              |
    //   |              |
    // (0, 0) ------- (1, 0)

    // (0, 0): Bottom-left corner.
    // (1, 0): Bottom-right corner.
    // (1, 1): Top-right corner.
    // (0, 1): Top-left corner.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ViewportEventData data;
    data.viewportX = viewportX;
    data.viewportY = viewportY;
    data.viewportW = viewportW;
    data.viewportH = viewportH;
    eventManager->emit(GameEvent::ViewportResized, data);
}

bool DisplayManager::screenshot(const std::filesystem::path &pathName) const {
    static constexpr size_t MAX_FILENAME = 256;
    static constexpr int CHANNELS = 3; // RGB

    char fileName[MAX_FILENAME];
    int fileIndex = 0;

    // Find free filename
    while (fileIndex < 9999) {
        const int result = snprintf(fileName, MAX_FILENAME, "%s/sdl-ball_%04d.png", // FIXME: use gameNAME from config
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

    // read viewport pixels
    const size_t pixelCount = viewportW * viewportH * CHANNELS;
    std::vector<Uint8> pixels(pixelCount);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(viewportX, viewportY, viewportW, viewportH, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    if (glGetError() != GL_NO_ERROR) {
        SDL_Log("Failed reading OpenGL framebuffer.");
        return false;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        pixels.data(), viewportW, viewportH, 24, viewportW * CHANNELS,
        0x0000FF, 0x00FF00, 0xFF0000, 0
    );
    if (!surface) {
        SDL_Log("Could not create SDL_Surface: %s", SDL_GetError());
        return false;
    }

    if (TextureUtilities::SDL_FlipSurfaceVertical(surface) != 0) {
        SDL_Log("Failed to flip surface vertically");
        SDL_FreeSurface(surface);
        return false;
    }

    if (IMG_SavePNG(surface, fileName) != 0) {
        SDL_Log("Failed writing PNG file: %s", IMG_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_FreeSurface(surface);
    return true;
}

bool DisplayManager::initOpenGL(const unsigned int flags) {
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

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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

    GLint drawBuffer;
    glGetIntegerv(GL_DRAW_BUFFER, &drawBuffer);
    SDL_Log("Draw Buffer nach CreateContext: %s", drawBuffer == GL_BACK ? "Back-Buffer" : "Front-Buffer");

    if (SDL_GL_MakeCurrent(sdlWindow, glcontext) < 0) {
        SDL_Log("Error:%s", SDL_GetError());
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return false;
    }

    int maj, min;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &maj);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &min);
    SDL_Log("=== Active OpenGL Context ===");
    SDL_Log("Context Version: %d.%d", maj, min);


    if (SDL_GL_SetSwapInterval(1) == 0) {
        SDL_Log("VSync aktiviert");
    } else {
        SDL_Log("VSync konnte nicht aktiviert werden: %s", SDL_GetError());
    }

    int doubleBuffered = 0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffered);
    SDL_Log("Double buffering %s", doubleBuffered ? "aktiviert" : "deaktiviert");

    /* Enable smooth shading */
    // glShadeModel(GL_SMOOTH);

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

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

DisplayManager::~DisplayManager() {
    SDL_DestroyWindow(sdlWindow);
    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();
}
