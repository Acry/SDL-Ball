#pragma once

#include <epoxy/gl.h>
#include <SDL2/SDL.h>

namespace TextureUtils {
    /**
     * Invertiert eine SDL_Surface entlang der Y-Achse, um die Unterschiede zwischen
     * SDL (0,0 oben links) und OpenGL (0,0 unten links) auszugleichen.
     *
     * @param surface Die Quell-Surface, die invertiert werden soll
     * @return Eine neue, Y-invertierte Surface oder nullptr bei Fehlern
     */
    SDL_Surface *invertSurfaceY(const SDL_Surface *surface);
    int SDL_FlipSurfaceVertical(SDL_Surface *surface);
    /**
     * Erstellt eine OpenGL-Textur aus einer SDL-Surface mit einheitlichen Parametern
     *
     * @param surface Die SDL-Surface, aus der die Textur erstellt wird
     * @param textureId Referenz auf die Textur-ID (wird gesetzt)
     * @param useClampMode True für GL_CLAMP Wrapping, False für Standard
     * @return True bei Erfolg, False bei Fehler
     */
    bool createGLTextureFromSurface(const SDL_Surface *surface, GLuint &textureId, bool useClampMode = false);
}
