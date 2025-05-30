// TextureUtilities.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include "TextureUtilities.h"

#define DEBUG_TEXTURE_FORMAT 0
/**
 * Erstellt eine OpenGL-Textur aus einer SDL-Surface mit einheitlichen Parametern
 *
 * @param surface Die SDL-Surface, aus der die Textur erstellt wird
 * @param textureId Referenz auf die Textur-ID (wird gesetzt)
 * @param useClampMode True für GL_CLAMP Wrapping, False für Standard
 * @return True bei Erfolg, False bei Fehler
 */
bool TextureUtilities::createGLTextureFromSurface(const SDL_Surface *surface, GLuint &textureId, bool useClampMode) {
    if (!surface) {
        SDL_Log("Error: No surface");
        return false;
    }

    GLenum format;
    GLenum type = GL_UNSIGNED_BYTE;
    switch (surface->format->format) {
        // 32-Bit RGBA Formate
        case SDL_PIXELFORMAT_RGBA8888:
        case SDL_PIXELFORMAT_ABGR8888:
        case SDL_PIXELFORMAT_ARGB8888:
        case SDL_PIXELFORMAT_BGRA8888:
            format = GL_RGBA;
            break;

        // 24-Bit RGB Formate
        case SDL_PIXELFORMAT_RGB888:
        case SDL_PIXELFORMAT_BGR888:
        case SDL_PIXELFORMAT_RGB24:
            format = GL_RGB;
            break;

        // 16-Bit Formate
        case SDL_PIXELFORMAT_RGB565:
        case SDL_PIXELFORMAT_BGR565:
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            break;

        case SDL_PIXELFORMAT_RGBA4444:
        case SDL_PIXELFORMAT_ARGB4444:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
            break;

        case SDL_PIXELFORMAT_RGBA5551:
        case SDL_PIXELFORMAT_ARGB1555:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
            break;

        // 8-Bit Format
        case SDL_PIXELFORMAT_INDEX8:
            format = GL_RED;  // Oder GL_LUMINANCE in älterem OpenGL
            break;

        default:
            SDL_Log("Error: unknown pixelformat: %s", SDL_GetPixelFormatName(surface->format->format));
            return false;
    }

#if DEBUG_TEXTURE_FORMAT
        SDL_Log("Pixelformat: %s", SDL_GetPixelFormatName(surface->format->format));
#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Textur erstellen
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Einheitliche Filtereinstellungen
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Optional Wrapping-Mode setzen
    if (useClampMode) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Textur-Daten laden
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                 format, type, surface->pixels);

    // Mipmap generieren
    // TODO: Mipmap-Generierung ist nicht immer notwendig (z.B. für UI-Elemente) - füge einen optionalen Parameter hinzu
    glGenerateMipmap(GL_TEXTURE_2D);

    if (const GLenum error = glGetError(); error != GL_NO_ERROR) {
        SDL_Log("GL-Error during texture generation: %d", error);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return false;
    }

    return true;
}

int TextureUtilities::SDL_FlipSurfaceVertical(SDL_Surface *surface) {
    // adjusted SDL3 - Code https://shorturl.at/iaAQF
    Uint8 *a, *b, *tmp;
    int i;

    // Überprüfe unterstützte Pixelformate
    if (surface->format->BitsPerPixel < 8) {
        return SDL_Unsupported();
    }

    // Nichts zu tun, wenn Höhe <= 1
    if (surface->h <= 1) {
        return 0;
    }

    // Sperre die Surface, falls nötig
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0) {
            return SDL_SetError("Could not lock surface: %s", SDL_GetError());
        }
    }

    // Tausche Zeilen
    a = (Uint8 *) surface->pixels;
    b = a + (surface->h - 1) * surface->pitch;
    tmp = static_cast<Uint8 *>(malloc(surface->pitch));
    if (!tmp) {
        if (SDL_MUSTLOCK(surface)) {
            SDL_UnlockSurface(surface);
        }
        return SDL_OutOfMemory();
    }

    for (i = surface->h / 2; i--;) {
        SDL_memcpy(tmp, a, surface->pitch);
        SDL_memcpy(a, b, surface->pitch);
        SDL_memcpy(b, tmp, surface->pitch);
        a += surface->pitch;
        b -= surface->pitch;
    }

    free(tmp);

    // Entsperre die Surface
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    return 0;
}
