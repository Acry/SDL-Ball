// TextureManager.cpp
#include <filesystem>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <epoxy/gl.h>

#include "TextureManager.h"

// Hilfsfunktion für die Konvertierung
TextureProperty getPropertyFromString(const std::string &key) {
    static const std::unordered_map<std::string, TextureProperty> propertyMap = {
        {"xoffset", TextureProperty::XOffset},
        {"yoffset", TextureProperty::YOffset},
        {"cols", TextureProperty::Cols},
        {"rows", TextureProperty::Rows},
        {"ticks", TextureProperty::Ticks},
        {"frames", TextureProperty::Frames},
        {"bidir", TextureProperty::Bidir},
        {"playing", TextureProperty::Playing},
        {"padding", TextureProperty::Padding},
        {"texcolor", TextureProperty::TexColor},
        {"parcolor", TextureProperty::ParColor},
        {"file", TextureProperty::FileName}
    };

    auto it = propertyMap.find(key);
    return it != propertyMap.end() ? it->second : TextureProperty::Unknown;
}

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

bool TextureManager::readTextureProperties(const std::filesystem::path &pathName, SpriteSheetAnimation &tex) {
    std::ifstream f(pathName);

    if (!f.is_open()) {
        SDL_Log("readTexProps: Cannot open '%s'", pathName.c_str());
        return false;
    }

    std::string line;
    while (std::getline(f, line)) {
        // Ignoriere empty lines and comments
        if (size_t commentPos = line.find("//"); commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        if (line.empty()) {
            continue;
        }

        // find key-value pairs
        size_t separatorPos = line.find('=');
        if (separatorPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, separatorPos);
        std::string value = line.substr(separatorPos + 1);

        // trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        TextureProperty property = getPropertyFromString(key);

        try {
            // Je nach Eigenschaft den Wert entsprechend verarbeiten
            switch (property) {
                case TextureProperty::XOffset:
                    tex.textureProperties.xoffset = std::stof(value);
                    break;
                case TextureProperty::YOffset:
                    tex.textureProperties.yoffset = std::stof(value);
                    break;
                case TextureProperty::Cols:
                    tex.textureProperties.cols = std::stoi(value);
                    break;
                case TextureProperty::Rows:
                    tex.textureProperties.rows = std::stoi(value);
                    break;
                case TextureProperty::Ticks:
                    tex.textureProperties.ticks = std::stoi(value);
                    break;
                case TextureProperty::Frames:
                    tex.textureProperties.frames = std::stoi(value);
                    break;
                case TextureProperty::Bidir:
                    tex.textureProperties.direction = std::stoi(value) != 0;
                    break;
                case TextureProperty::Playing:
                    tex.textureProperties.playing = std::stoi(value) != 0;
                    break;
                case TextureProperty::Padding:
                    tex.textureProperties.padding = std::stoi(value) != 0;
                    break;
                case TextureProperty::TexColor:
                    if (value.length() >= 8) {
                        constexpr float normalizer = 1.0f / 255.0f;
                        tex.textureProperties.glTexColorInfo[0] =
                                static_cast<float>(std::stoi(value.substr(0, 2), nullptr, 16)) * normalizer;
                        tex.textureProperties.glTexColorInfo[1] =
                                static_cast<float>(std::stoi(value.substr(2, 2), nullptr, 16)) * normalizer;
                        tex.textureProperties.glTexColorInfo[2] =
                                static_cast<float>(std::stoi(value.substr(4, 2), nullptr, 16)) * normalizer;
                        tex.textureProperties.glTexColorInfo[3] =
                                static_cast<float>(std::stoi(value.substr(6, 2), nullptr, 16)) * normalizer;
                    }
                    break;
                case TextureProperty::ParColor:
                    if (value.length() >= 6) {
                        constexpr float normalizer = 1.0f / 255.0f;
                        tex.textureProperties.glParColorInfo[0] =
                                static_cast<float>(std::stoi(value.substr(0, 2), nullptr, 16)) * normalizer;
                        tex.textureProperties.glParColorInfo[1] =
                                static_cast<float>(std::stoi(value.substr(2, 2), nullptr, 16)) * normalizer;
                        tex.textureProperties.glParColorInfo[2] =
                                static_cast<float>(std::stoi(value.substr(4, 2), nullptr, 16)) * normalizer;
                    }
                    break;
                case TextureProperty::FileName:
                    tex.textureProperties.fileName = value;
                    break;
                case TextureProperty::Unknown:
                    SDL_Log("Warning: '%s' hat unknown property key: '%s' with value: '%s'",
                            pathName.c_str(), key.c_str(), value.c_str());
                    break;
            }
        } catch (const std::exception &e) {
            SDL_Log("Error reading key: '%s' with value '%s': %s",
                    key.c_str(), value.c_str(), e.what());
        }
    }

    return true;
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
        readTextureProperties(txtPath, *newTexture);
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

SpriteSheetAnimation *TextureManager::getPaddleTexture(PaddleTexture type) const {
    size_t index = static_cast<size_t>(type);
    if (index >= paddleTextures.size()) return nullptr;
    return paddleTextures[index];
}

SpriteSheetAnimation *TextureManager::getBallTexture(BallTexture type) const {
    size_t index = static_cast<size_t>(type);
    if (index >= ballTextures.size()) return nullptr;
    return ballTextures[index];
}

SpriteSheetAnimation *TextureManager::getBrickTexture(BrickTexture type) const {
    size_t index = static_cast<size_t>(type);
    if (index >= brickTextures.size()) return nullptr;
    return brickTextures[index];
}

SpriteSheetAnimation *TextureManager::getPowerUpTexture(PowerUpTexture type) const {
    size_t index = static_cast<size_t>(type);
    if (index >= powerUpTextures.size()) return nullptr;
    return powerUpTextures[index];
}

SpriteSheetAnimation *TextureManager::getEffectTexture(EffectTexture type) const {
    size_t index = static_cast<size_t>(type);
    if (index >= effectTextures.size()) return nullptr;
    return effectTextures[index];
}

SpriteSheetAnimation *TextureManager::getMiscTexture(MiscTexture type) const {
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
    // 1. load properties
    std::filesystem::path propsPath = basePath + ".txt";
    bool hasProps = false;
    bool textureLoaded = false;
    std::filesystem::path actualImagePath;


    if (std::filesystem::exists(propsPath)) {
        hasProps = true;
        if (!readTextureProperties(propsPath, animation)) {
            SDL_Log("Warning: no properties '%s'", propsPath.c_str());
        }

        // 2. get image name from properties
        if (!animation.textureProperties.fileName.empty()) {
            const std::filesystem::path basePath_path(basePath);
            const std::filesystem::path themeRoot = basePath_path.parent_path().parent_path();
            actualImagePath = themeRoot / "gfx" / animation.textureProperties.fileName;

            if (std::filesystem::exists(actualImagePath)) {
                if (load(actualImagePath, animation)) {
                    textureLoaded = true;
                } else {
                    SDL_Log("Error: the image in the property couldn't be read: '%s'",
                            actualImagePath.c_str());
                }
            }
        }
    }

    // 3. try to ge an image from basename
    if (!textureLoaded) {
        const std::vector<std::string> supportedFormats = {
            ".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp", ".jxl", ".avif"
        };

        for (const auto &format : supportedFormats) {
            if (std::filesystem::path texturePath = basePath + format; std::filesystem::exists(texturePath)) {
                if (load(texturePath, animation)) {
                    textureLoaded = true;
                    actualImagePath = texturePath;
                    break;
                }
            }
        }
    }

    // 4. Fehlerbehandlung nach den genannten Regeln
    if (!textureLoaded) {
        if (hasProps && !animation.textureProperties.fileName.empty()) {
            SDL_Log("Error: Die in der Eigenschaftsdatei angegebene Bilddatei existiert nicht: '%s'",
                    animation.textureProperties.fileName.c_str());
        } else if (hasProps) {
            SDL_Log("Error: Eigenschaftsdatei gefunden, aber keine 'file=' Angabe oder die Datei existiert nicht: '%s'",
                    propsPath.c_str());
        } else {
            SDL_Log("Error: Keine Bilddatei für '%s' in unterstützten Formaten gefunden", basePath.c_str());
        }
        return false;
    }

    // ja, ich weiß nicht. das hat hier nichts zu suchen. mal sehen
    // 5. Standard-Eigenschaften setzen, wenn keine Eigenschaften geladen wurden
    if (!hasProps) {
        SDL_Log("Hinweis: Keine Eigenschaftsdatei für '%s' gefunden", actualImagePath.c_str());

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

        // Standardfarben setzen (weiß)
        for (int i = 0; i < 3; i++) {
            animation.textureProperties.glTexColorInfo[i] = 1.0f;
            animation.textureProperties.glParColorInfo[i] = 1.0f;
        }
        animation.textureProperties.glTexColorInfo[3] = 1.0f; // Alpha = 1.0
    }

    return true;
}
