// TextureTutilities.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include "TextureUtilities.h"

#define DEBUG_TEXTURE_FORMAT 0
namespace TextureUtils {
    /**
     * Invertiert eine SDL_Surface entlang der Y-Achse, um die Unterschiede zwischen
     * SDL (0,0 oben links) und OpenGL (0,0 unten links) auszugleichen.
     *
     * @param surface Die Quell-Surface, die invertiert werden soll
     * @return Eine neue, Y-invertierte Surface oder nullptr bei Fehlern
     */
    SDL_Surface *invertSurfaceY(const SDL_Surface *surface) {
        SDL_Surface *invertedSurface = SDL_CreateRGBSurfaceWithFormat(
            0, surface->w, surface->h,
            surface->format->BitsPerPixel,
            surface->format->format
        );

        if (!invertedSurface) {
            SDL_Log("Fehler beim Erstellen der invertierten Surface: %s", SDL_GetError());
            return nullptr;
        }

        int bytesPerPixel = surface->format->BytesPerPixel;

        for (int y = 0; y < surface->h; y++) {
            uint8_t *srcRow = static_cast<uint8_t *>(surface->pixels) + y * surface->pitch;
            uint8_t *dstRow = static_cast<uint8_t *>(invertedSurface->pixels) +
                              (surface->h - 1 - y) * invertedSurface->pitch;
            memcpy(dstRow, srcRow, surface->w * bytesPerPixel);
        }

        return invertedSurface;
    }

    /**
     * Erstellt eine OpenGL-Textur aus einer SDL-Surface mit einheitlichen Parametern
     *
     * @param surface Die SDL-Surface, aus der die Textur erstellt wird
     * @param textureId Referenz auf die Textur-ID (wird gesetzt)
     * @param useClampMode True für GL_CLAMP Wrapping, False für Standard
     * @return True bei Erfolg, False bei Fehler
     */
    bool createGLTextureFromSurface(const SDL_Surface *surface, GLuint &textureId, bool useClampMode) {
        if (!surface) {
            SDL_Log("Fehler: Keine gültige Surface übergeben");
            return false;
        }

        // GL-Format basierend auf der Surface bestimmen
        GLenum format;
        switch (surface->format->format) {
            case SDL_PIXELFORMAT_RGBA8888:
            case SDL_PIXELFORMAT_ABGR8888:
                format = GL_RGBA;
                break;
            case SDL_PIXELFORMAT_RGB888:
            case SDL_PIXELFORMAT_BGR888:
                format = GL_RGB;
                break;
            default:
                SDL_Log("Nicht unterstütztes Pixelformat: %s", SDL_GetPixelFormatName(surface->format->format));
                return false;
        }
#if DEBUG_TEXTURE_FORMAT
        SDL_Log("Pixelformat: %s", SDL_GetPixelFormatName(surface->format->format));
#endif
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Keine zusätzliche Ausrichtung
        // Textur erstellen
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Einheitliche Filtereinstellungen
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Optional Wrapping-Mode setzen
        if (useClampMode) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }

        // Textur-Daten laden
        glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0,
                     format, GL_UNSIGNED_BYTE, surface->pixels);

        // Mipmap generieren
        glGenerateMipmap(GL_TEXTURE_2D);

        // Überprüfen auf OpenGL-Fehler
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            SDL_Log("OpenGL-Fehler beim Erstellen der Textur: %d", error);
            glDeleteTextures(1, &textureId);
            return false;
        }

        return true;
    }

    int SDL_FlipSurfaceVertical(SDL_Surface *surface) {
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
                return SDL_SetError("Konnte Surface nicht sperren: %s", SDL_GetError());
            }
        }

        // Tausche Zeilen
        a = (Uint8 *)surface->pixels;
        b = a + (surface->h - 1) * surface->pitch;
        tmp = static_cast<Uint8*>(malloc(surface->pitch));
        if (!tmp) {
            if (SDL_MUSTLOCK(surface)) {
                SDL_UnlockSurface(surface);
            }
            return SDL_OutOfMemory();
        }

        for (i = surface->h / 2; i--; ) {
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

}
