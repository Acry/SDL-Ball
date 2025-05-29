// TextureTutilities.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include "TextureUtilities.h"

namespace TextureUtils {
    /**
     * Invertiert eine SDL_Surface entlang der Y-Achse, um die Unterschiede zwischen
     * SDL (0,0 oben links) und OpenGL (0,0 unten links) auszugleichen.
     *
     * @param surface Die Quell-Surface, die invertiert werden soll
     * @return Eine neue, Y-invertierte Surface oder nullptr bei Fehlern
     */
    SDL_Surface *invertSurfaceY(const SDL_Surface *surface) {
        // Erstelle eine neue Surface mit den gleichen Eigenschaften
        SDL_Surface *invertedSurface = SDL_CreateRGBSurfaceWithFormat(
            0, surface->w, surface->h,
            surface->format->BitsPerPixel,
            surface->format->format
        );

        if (!invertedSurface) {
            SDL_Log("Fehler beim Erstellen der invertierten Surface: %s", SDL_GetError());
            return nullptr;
        }

        // Berechne Bytes pro Pixel
        int bytesPerPixel = surface->format->BytesPerPixel;

        // Kopiere Pixel Zeile für Zeile in umgekehrter Reihenfolge
        for (int y = 0; y < surface->h; y++) {
            uint8_t *srcRow = static_cast<uint8_t *>(surface->pixels) + y * surface->pitch;
            uint8_t *dstRow = static_cast<uint8_t *>(invertedSurface->pixels) +
                              (surface->h - 1 - y) * invertedSurface->pitch;

            // Kopiere die gesamte Zeile
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
        GLint glFormat = GL_RGBA;
        switch (surface->format->BytesPerPixel) {
            case 3:
                glFormat = GL_RGB;
                break;
            case 4:
                glFormat = GL_RGBA;
                break;
            default:
                SDL_Log("Warnung: Unbekanntes Surface-Format, verwende RGBA");
                break;
        }

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
        glTexImage2D(GL_TEXTURE_2D, 0, glFormat, surface->w, surface->h, 0,
                     glFormat, GL_UNSIGNED_BYTE, surface->pixels);

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
}

