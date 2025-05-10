// TextureManager.cpp
#include "TextureManager.h"
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <epoxy/gl.h>
#include "SettingsManager.h"
#include "ThemeManager.h"

extern settings setting;
extern TextureManager textureManager;
extern ThemeManager themeManager;

bool TextureManager::load(const std::string& file, Texture& tex) {
    SDL_Surface* temp = nullptr;
    GLint maxTexSize;
    GLuint glFormat = GL_RGBA;

    if (file.substr(file.length() - 3, 3) == "jpg") {
        glFormat = GL_RGB;
    }

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

    temp = IMG_Load(file.data());

    if (temp == nullptr) {
        SDL_Log("Texture manager:%s :%s", file.c_str(), SDL_GetError());
        SDL_FreeSurface(temp);
        return false;
    }

    if (temp->w > maxTexSize) {
        SDL_Log("Texture manager: '%s' texturesize too large.", file.c_str());
        SDL_FreeSurface(temp);
        return false;
    }

    glGenTextures(1, &tex.prop.texture);
    glBindTexture(GL_TEXTURE_2D, tex.prop.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, temp->w, temp->h, 0, glFormat, GL_UNSIGNED_BYTE, temp->pixels);

    tex.prop.pxw = temp->w;
    tex.prop.pxh = temp->h;
    SDL_FreeSurface(temp);

    return true;
}

void TextureManager::readTexProps(std::string fileName, Texture& tex) {
    char rgba[4][5];
    std::ifstream f;
    std::string set, val;
    f.open(fileName.data());

    if (f.is_open()) {
        std::string line;
        while (!f.eof()) {
            getline(f, line);
            if (line.find('=') != std::string::npos) {
                set = line.substr(0, line.find('='));
                val = line.substr(line.find('=') + 1);
                if (set == "xoffset") {
                    tex.prop.xoffset = atof(val.data());
                } else if (set == "yoffset") {
                    tex.prop.yoffset = atof(val.data());
                } else if (set == "cols") {
                    tex.prop.cols = atoi(val.data());
                } else if (set == "rows") {
                    tex.prop.rows = atoi(val.data());
                } else if (set == "ticks") {
                    tex.prop.ticks = atoi(val.data());
                } else if (set == "frames") {
                    tex.prop.frames = atoi(val.data());
                } else if (set == "bidir") {
                    tex.prop.bidir = atoi(val.data());
                } else if (set == "playing") {
                    tex.prop.playing = atoi(val.data());
                } else if (set == "padding") {
                    tex.prop.padding = atoi(val.data());
                } else if (set == "texcolor") {
                    sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                    sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                    sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                    sprintf(rgba[3], "0x%c%c", val[6], val[7]);
                    tex.prop.glTexColorInfo[0] = 0.003921569f * strtol(rgba[0], nullptr, 16);
                    tex.prop.glTexColorInfo[1] = 0.003921569f * strtol(rgba[1], nullptr, 16);
                    tex.prop.glTexColorInfo[2] = 0.003921569f * strtol(rgba[2], nullptr, 16);
                    tex.prop.glTexColorInfo[3] = 0.003921569f * strtol(rgba[3], nullptr, 16);
                } else if (set == "parcolor") {
                    sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                    sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                    sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                    tex.prop.glParColorInfo[0] = 0.003921569f * strtol(rgba[0], nullptr, 16);
                    tex.prop.glParColorInfo[1] = 0.003921569f * strtol(rgba[1], nullptr, 16);
                    tex.prop.glParColorInfo[2] = 0.003921569f * strtol(rgba[2], nullptr, 16);
                } else if (set == "file") {
                    tex.prop.fileName = val;
                } else {
                    SDL_Log("Error: '%s' invalid setting '%s' with value '%s'", fileName.c_str(), set.c_str(), val.c_str());
                }
            }
        }

        if (tex.prop.fileName.length() > 1) {
            std::string name = "gfx/" + tex.prop.fileName;
            load(themeManager.getThemeFilePath(name, setting.gfxTheme), tex);
        }
    } else {
        SDL_Log("readTexProps: Cannot open '%s'", fileName.c_str());
    }
}