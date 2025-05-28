// DisplayManager.cpp
#include <iostream>

#include "Display.hpp"

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
    constexpr float target_ratio = 4.0f / 3.0f;
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

    // The near and far values used by glOrtho are only distances and (assuming that the eye is located at the origin)
    // sets up a projection where -nearVal is on the front clipping plane and -farVal is the location of the far
    // clipping plane.
    // Flipping the y-axis with glOrtho only affects the projection
    // The viewport’s origin (0, 0) remains at the bottom-left in window
    // coordinates unless you modify the viewport or use a custom framebuffer.
    glViewport(viewportX, viewportY, viewportW, viewportH);
    glScissor(viewportX, viewportY, viewportW, viewportH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // NOTE: Accidentaly did not flipped y-axis projection
    // That means, most Texture Coordinates are flipped.
    // I will resolve this. Since the y-axis is openGL default,
    // I want the UV coordinates to be OpenGL-default as well.
    // That will need work in the texture manager, I need to flip the image on loading.

    // NDC projection: OpenGL
    // Von -1 bis +1 in NDC
    // Die Werte für near und far in glOrtho sind tatsächlich etwas verwirrend.
    // Die Z-Werte -1.0f (near) und 1.0f (far) definieren den sichtbaren Tiefenbereich.
    // -1.0f (near) ist die hintere Clipping-Ebene (weiter vom Betrachter entfernt)
    // und 1.0f (far) ist die vordere Clipping-Ebene (näher am Betrachter).
    // Z = -1.0f wird zu 1.0 im Tiefenpuffer (am weitesten entfernt)
    // Z = 1.0f wird zu 0.0 im Tiefenpuffer (am nächsten zum Betrachter)
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // left, right, bottom, top, near, far
    //         +1
    //         ^
    //         |
    // -1 <----+----> +1
    //         |
    //        -1

    // In OpenGL, texture coordinates (u, v) typically have (0, 0) at the bottom-left of the texture
    // (0, 1) ------ ( 1, 1)
    //   |              |
    //   |              |
    //   |              |
    //   (0, 0) ------ (1, 0)
    //
    //  (0, 0): Bottom-left corner.
    //  (1, 0): Bottom-right corner.
    //  (0, 1): Top-left corner.
    //  (1, 1): Top-right corner.

    //         -1
    //         ^
    //         |
    // -1 <----+----> +1
    //         |
    //        +1
    // (0, 0) ------ ( 1, 0)
    //   |              |
    //   |              |
    //   |              |
    //   (0, 1) ------ (1, 1)
    // flipped y-axis projection
    // glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // Bottom-left vertex, top of texture
    // glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f); // Bottom-right vertex, top of texture
    // glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f); // Top-right vertex, bottom of texture
    // glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f); // Top-left vertex, bottom of texture
    // glOrtho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // When using SDL2 with OpenGL and you don’t flip the y-axis in the projection (e.g., using glOrtho(-1, 1, -1, 1, -1, 1) for standard OpenGL NDC where positive y is up), you will likely need to flip the v-coordinates of your texture coordinates to align SDL2-loaded textures correctly. This is because SDL2 and OpenGL have different conventions for texture origins:
    // SDL2: Assumes the texture/image origin (0, 0) is at the top-left, with positive y extending downward.
    // OpenGL: Defines texture coordinates with (u, v) = (0, 0) at the bottom-left, with positive v extending upward.
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

    /* Enables Depth Testing */
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

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
    // TODO: MouseManager
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_Quit();
}
