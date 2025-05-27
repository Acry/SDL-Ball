// TextureManager.cpp
#include <filesystem>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <epoxy/gl.h>

#include "TextureManager.h"

GLuint getGLFormat(const SDL_Surface *surface) {
    if (!surface) {
        return GL_RGBA;
    }

    switch (surface->format->BytesPerPixel) {
        case 3:
            return GL_RGB;
        default:
            return GL_RGBA;
    }
}

TextureManager::TextureManager()
    : maxTexSize(0) {
    paddleTextures.fill(nullptr);
    ballTextures.fill(nullptr);
    brickTextures.fill(nullptr);
    powerUpTextures.fill(nullptr);
    miscTextures.fill(nullptr);
    effectTextures.fill(nullptr);
    titleTextures.fill(nullptr);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
}

bool TextureManager::load(const std::filesystem::path &pathName, SpriteSheetAnimation &tex) const {
    SDL_Surface *tempSurface = IMG_Load(pathName.c_str());
    if (!tempSurface) {
        SDL_Log("Error: no image file: %s", pathName.c_str());
        return false;
    }
    if (tempSurface->w > maxTexSize || tempSurface->h > maxTexSize) {
        SDL_Log("Error: image too big: %s (%dx%d)", pathName.c_str(), tempSurface->w, tempSurface->h);
        SDL_FreeSurface(tempSurface);
        return false;
    }
    const GLint glFormat = getGLFormat(tempSurface);

    glGenTextures(1, &tex.textureProperties.texture);
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, tempSurface->w, tempSurface->h, 0, glFormat, GL_UNSIGNED_BYTE,
                 tempSurface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    tex.textureProperties.pxw = tempSurface->w;
    tex.textureProperties.pxh = tempSurface->h;
    SDL_FreeSurface(tempSurface);

    return true;
}

bool TextureManager::readTexProps(const std::filesystem::path &pathName, SpriteSheetAnimation &tex) const {
    std::ifstream f;
    std::string set, val;
    f.open(pathName.c_str());

    if (!f.is_open()) {
        SDL_Log("readTexProps: Cannot open '%s'", pathName.c_str());
        return false;
    }

    char rgba[4][5];
    if (f.is_open()) {
        std::string line;
        while (!f.eof()) {
            getline(f, line);
            if (line.find('=') != std::string::npos) {
                set = line.substr(0, line.find('='));
                val = line.substr(line.find('=') + 1);
                if (set == "xoffset") {
                    tex.textureProperties.xoffset = atof(val.data());
                } else if (set == "yoffset") {
                    tex.textureProperties.yoffset = atof(val.data());
                } else if (set == "cols") {
                    tex.textureProperties.cols = atoi(val.data());
                } else if (set == "rows") {
                    tex.textureProperties.rows = atoi(val.data());
                } else if (set == "ticks") {
                    tex.textureProperties.ticks = atoi(val.data());
                } else if (set == "frames") {
                    tex.textureProperties.frames = atoi(val.data());
                } else if (set == "bidir") {
                    tex.textureProperties.direction = atoi(val.data());
                } else if (set == "playing") {
                    tex.textureProperties.playing = atoi(val.data());
                } else if (set == "padding") {
                    tex.textureProperties.padding = atoi(val.data());
                } else if (set == "texcolor") {
                    sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                    sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                    sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                    sprintf(rgba[3], "0x%c%c", val[6], val[7]);
                    tex.textureProperties.glTexColorInfo[0] = 0.003921569f * strtol(rgba[0], nullptr, 16);
                    tex.textureProperties.glTexColorInfo[1] = 0.003921569f * strtol(rgba[1], nullptr, 16);
                    tex.textureProperties.glTexColorInfo[2] = 0.003921569f * strtol(rgba[2], nullptr, 16);
                    tex.textureProperties.glTexColorInfo[3] = 0.003921569f * strtol(rgba[3], nullptr, 16);
                } else if (set == "parcolor") {
                    sprintf(rgba[0], "0x%c%c", val[0], val[1]);
                    sprintf(rgba[1], "0x%c%c", val[2], val[3]);
                    sprintf(rgba[2], "0x%c%c", val[4], val[5]);
                    tex.textureProperties.glParColorInfo[0] = 0.003921569f * strtol(rgba[0], nullptr, 16);
                    tex.textureProperties.glParColorInfo[1] = 0.003921569f * strtol(rgba[1], nullptr, 16);
                    tex.textureProperties.glParColorInfo[2] = 0.003921569f * strtol(rgba[2], nullptr, 16);
                } else if (set == "file") {
                    tex.textureProperties.fileName = val;
                } else {
                    SDL_Log("Error: '%s' invalid setting '%s' with value '%s'", pathName.c_str(), set.c_str(),
                            val.c_str());
                }
            }
        }

        // Ich weiß momentan nicht ob das hier noch gebraucht wird
        if (tex.textureProperties.fileName.length() > 1) {
            std::string name = "gfx/" + tex.textureProperties.fileName;
            // load(themeManager.getThemeFilePath(name, setting.gfxTheme), tex);
        }
    } else {
        SDL_Log("readTexProps: Cannot open '%s'", pathName.c_str());
    }
}

TextureManager::~TextureManager() {
    clearTheme();
}

// bekommt z.B.: /home/carsten/sources/extern/GameDev/example_games/breakout/SDL-Ball/SDL-Ball-Project/themes/default
bool TextureManager::setSpriteTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    // Ressourcen freigeben
    clearTheme();

    // Neuen Pfad setzen
    currentTheme = themeName;

    // Texturen neu laden
    if (!loadAllGameTextures()) {
        SDL_Log("Fehler beim Laden der Texturen für Theme: %s", currentTheme.c_str());
        return false;
    }

    return true;
}

void TextureManager::clearTheme() {
    for (auto &pair: textureCache) {
        if (pair.second) {
            glDeleteTextures(1, &pair.second->textureProperties.texture);
        }
    }

    // Cache leeren
    textureCache.clear();

    // Array-Referenzen zurücksetzen
    for (auto &tex: paddleTextures) tex = nullptr;
    for (auto &tex: ballTextures) tex = nullptr;
    for (auto &tex: brickTextures) tex = nullptr;
    for (auto &tex: powerUpTextures) tex = nullptr;
    for (auto &tex: miscTextures) tex = nullptr;
    for (auto &tex: effectTextures) tex = nullptr;
    for (auto &tex: titleTextures) tex = nullptr;
}

SpriteSheetAnimation *TextureManager::loadAndCacheTexture(const std::string &path, const bool forceReload) {
    if (!forceReload && textureCache.contains(path)) {
        return textureCache[path].get();
    }

    auto newTexture = std::make_unique<SpriteSheetAnimation>();

    const std::vector<std::string> supportedFormats = {
        ".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp", ".jxl", ".avif"
    };

    const std::filesystem::path txtPath = currentTheme + "/" + path + ".txt";

    bool textureLoaded = false;
    std::filesystem::path loadedPath;

    for (const auto &format: supportedFormats) {
        std::filesystem::path imagePath = std::filesystem::path(currentTheme) / path;
        imagePath += format;

        if (std::filesystem::exists(imagePath)) {
            if (load(imagePath, *newTexture)) {
                textureLoaded = true;
                loadedPath = imagePath;
                break;
            }
        }
    }
    if (!textureLoaded) {
        SDL_Log("Fehler: Keine Bilddatei für %s in unterstützten Formaten gefunden", path.c_str());
        return nullptr;
    }

    // Eigenschaften laden, falls vorhanden
    if (std::filesystem::exists(txtPath)) {
        readTexProps(txtPath, *newTexture);
    } else {
        SDL_Log("Hinweis: Keine Eigenschaftsdatei für %s gefunden", path.c_str());
        // Standardwerte für Textureigenschaften setzen
        newTexture->textureProperties.ticks = 1000;
        newTexture->textureProperties.cols = 1;
        newTexture->textureProperties.rows = 1;
        newTexture->textureProperties.xoffset = 1.0f;
        newTexture->textureProperties.yoffset = 1.0f;
        newTexture->textureProperties.frames = 1;
        newTexture->textureProperties.playing = false;
        newTexture->textureProperties.direction = false;
        newTexture->textureProperties.padding = false;
    }

    // In Cache einfügen und Pointer zurückgeben
    textureCache[path] = std::move(newTexture);
    return textureCache[path].get();
}

SpriteSheetAnimation *TextureManager::getTexture(const std::string &texturePath, bool forceReload) {
    return loadAndCacheTexture(texturePath, forceReload);
}

SpriteSheetAnimation *TextureManager::getPaddleTexture(PaddleTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= paddleTextures.size()) return nullptr;
    return paddleTextures[index];
}

SpriteSheetAnimation *TextureManager::getBallTexture(BallTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= ballTextures.size()) return nullptr;
    return ballTextures[index];
}

SpriteSheetAnimation *TextureManager::getBrickTexture(BrickTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= brickTextures.size()) return nullptr;
    return brickTextures[index];
}

SpriteSheetAnimation *TextureManager::getPowerUpTexture(PowerUpTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= powerUpTextures.size()) return nullptr;
    return powerUpTextures[index];
}

SpriteSheetAnimation *TextureManager::getEffectTexture(EffectTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= effectTextures.size()) return nullptr;
    return effectTextures[index];
}

SpriteSheetAnimation *TextureManager::getMiscTexture(MiscTexture type) {
    size_t index = static_cast<size_t>(type);
    if (index >= miscTextures.size()) return nullptr;
    return miscTextures[index];
}

bool TextureManager::loadAllGameTextures() {
    // Paddle-Texturen laden
    paddleTextures[static_cast<size_t>(PaddleTexture::Base)] = loadAndCacheTexture("gfx/paddle/base");
    paddleTextures[static_cast<size_t>(PaddleTexture::Glue)] = loadAndCacheTexture("gfx/paddle/glue");
    paddleTextures[static_cast<size_t>(PaddleTexture::Gun)] = loadAndCacheTexture("gfx/paddle/gun");

    // Ball-Texturen laden
    ballTextures[static_cast<size_t>(BallTexture::Normal)] = loadAndCacheTexture("gfx/ball/normal");
    ballTextures[static_cast<size_t>(BallTexture::Fireball)] = loadAndCacheTexture("gfx/ball/fireball");

    // Brick-Texturen laden
    brickTextures[static_cast<size_t>(BrickTexture::Explosive)] = loadAndCacheTexture("gfx/brick/explosive");
    brickTextures[static_cast<size_t>(BrickTexture::Base)] = loadAndCacheTexture("gfx/brick/base");
    brickTextures[static_cast<size_t>(BrickTexture::Cement)] = loadAndCacheTexture("gfx/brick/cement");
    brickTextures[static_cast<size_t>(BrickTexture::Doom)] = loadAndCacheTexture("gfx/brick/doom");
    brickTextures[static_cast<size_t>(BrickTexture::Glass)] = loadAndCacheTexture("gfx/brick/glass");
    brickTextures[static_cast<size_t>(BrickTexture::Invisible)] = loadAndCacheTexture("gfx/brick/invisible");
    brickTextures[static_cast<size_t>(BrickTexture::Blue)] = loadAndCacheTexture("gfx/brick/blue");
    brickTextures[static_cast<size_t>(BrickTexture::Yellow)] = loadAndCacheTexture("gfx/brick/yellow");
    brickTextures[static_cast<size_t>(BrickTexture::Green)] = loadAndCacheTexture("gfx/brick/green");
    brickTextures[static_cast<size_t>(BrickTexture::Grey)] = loadAndCacheTexture("gfx/brick/grey");
    brickTextures[static_cast<size_t>(BrickTexture::Purple)] = loadAndCacheTexture("gfx/brick/purple");
    brickTextures[static_cast<size_t>(BrickTexture::White)] = loadAndCacheTexture("gfx/brick/white");
    brickTextures[static_cast<size_t>(BrickTexture::Red)] = loadAndCacheTexture("gfx/brick/red");

    // Powerup-Texturen laden
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Coin)] = loadAndCacheTexture("gfx/powerup/coin");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Glue)] = loadAndCacheTexture("gfx/powerup/glue");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Multiball)] = loadAndCacheTexture("gfx/powerup/multiball");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::BigBall)] = loadAndCacheTexture("gfx/powerup/bigball");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::NormalBall)] = loadAndCacheTexture("gfx/powerup/normalball");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::SmallBall)] = loadAndCacheTexture("gfx/powerup/smallball");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Aim)] = loadAndCacheTexture("gfx/powerup/aim");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Explosive)] = loadAndCacheTexture("gfx/powerup/explosive");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Gun)] = loadAndCacheTexture("gfx/powerup/gun");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Laser)] = loadAndCacheTexture("gfx/powerup/laser");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Life)] = loadAndCacheTexture("gfx/powerup/life");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Die)] = loadAndCacheTexture("gfx/powerup/die");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Drop)] = loadAndCacheTexture("gfx/powerup/drop");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Detonate)] = loadAndCacheTexture("gfx/powerup/detonate");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::ExplosiveGrow)] = loadAndCacheTexture(
        "gfx/powerup/explosive-grow");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::EasyBrick)] = loadAndCacheTexture("gfx/powerup/easybrick");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::NextLevel)] = loadAndCacheTexture("gfx/powerup/nextlevel");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::AimHelp)] = loadAndCacheTexture("gfx/powerup/aimhelp");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::GrowPaddle)] = loadAndCacheTexture("gfx/powerup/growbat");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::ShrinkPaddle)] = loadAndCacheTexture("gfx/powerup/shrinkbat");
    powerUpTextures[static_cast<size_t>(PowerUpTexture::Bullet)] = loadAndCacheTexture("gfx/powerup/bullet");

    // Misc Textures
    miscTextures[static_cast<size_t>(MiscTexture::Border)] = loadAndCacheTexture("gfx/border");

    // Effects Textures
    effectTextures[static_cast<size_t>(EffectTexture::Tail)] = loadAndCacheTexture("gfx/effects/tail");
    effectTextures[static_cast<size_t>(EffectTexture::Particle)] = loadAndCacheTexture("gfx/effects/particle");

    // Title Textures
    titleTextures[static_cast<size_t>(TitleTexture::Title)] = loadAndCacheTexture("gfx/title/title");
    return true;
}

bool TextureManager::loadTextureWithProperties(const std::string &basePath, SpriteSheetAnimation &animation) const {
    // 1. Prüfen, ob die Basispfad ohne Dateiendung übergeben wurde
    std::filesystem::path texturePath;
    std::filesystem::path propsPath = basePath + ".txt";
    bool textureLoaded = false;

    // 2. Unterstützte Bildformate durchprobieren
    const std::vector<std::string> supportedFormats = {
        ".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp"
    };

    for (const auto &format: supportedFormats) {
        texturePath = basePath + format;
        if (std::filesystem::exists(texturePath)) {
            if (load(texturePath, animation)) {
                textureLoaded = true;
                break;
            }
        }
    }

    // 3. Eigenschaften aus Textdatei laden
    bool hasProps = false;
    if (std::filesystem::exists(propsPath)) {
        readTexProps(propsPath, animation);
        hasProps = true;

        // 4. Wenn in den Eigenschaften ein "file=" Attribut existiert, dieses alternativ laden
        if (!animation.textureProperties.fileName.empty() && !textureLoaded) {
            std::filesystem::path basePath_path(basePath);
            std::filesystem::path themeRoot = basePath_path.parent_path().parent_path();
            std::filesystem::path absolutePath = themeRoot / animation.textureProperties.fileName;
            std::string altPath = absolutePath.string();
            if (load(altPath, animation)) {
                textureLoaded = true;
            }
        }
    }

    // 5. Fehlerbehandlung nach den genannten Regeln
    if (!textureLoaded) {
        if (hasProps && !animation.textureProperties.fileName.empty()) {
            SDL_Log("Fehler: Die in der Eigenschaftsdatei angegebene Bilddatei existiert nicht: %s",
                    animation.textureProperties.fileName.c_str());
        } else if (hasProps) {
            SDL_Log("Fehler: Eigenschaftsdatei gefunden, aber keine 'file=' Angabe oder die Datei existiert nicht: %s",
                    propsPath.c_str());
        } else {
            SDL_Log("Fehler: Keine Bilddatei für %s in unterstützten Formaten gefunden", basePath.c_str());
        }
        return false;
    }

    // 6. Standard-Eigenschaften setzen, wenn keine Eigenschaften geladen wurden
    if (!hasProps) {
        SDL_Log("Hinweis: Keine Eigenschaftsdatei für %s gefunden", texturePath.c_str());

        // Ist das clever?
        // Standardwerte für Textureigenschaften setzen
        animation.textureProperties.ticks = 1000;
        animation.textureProperties.cols = 1;
        animation.textureProperties.rows = 1;
        animation.textureProperties.xoffset = 1.0f;
        animation.textureProperties.yoffset = 1.0f;
        animation.textureProperties.frames = 1;
        animation.textureProperties.playing = false;
        animation.textureProperties.direction = false;
        animation.textureProperties.padding = false;

        // Standardfarben setzen
        for (int i = 0; i < 3; i++) {
            animation.textureProperties.glTexColorInfo[i] = 1.0f;
            animation.textureProperties.glParColorInfo[i] = 1.0f;
        }
        animation.textureProperties.glTexColorInfo[3] = 1.0f; // Alpha = 1.0
    }

    return true;
}
