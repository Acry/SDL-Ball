// TextureManager.cpp
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include <fstream>
#include "TextureUtilities.h"
#include "TextureManager.h"

#include <ranges>

#define DEBUG_SORT_BACKGROUNDS 0

TextureProperty getPropertyFromString(const std::string &key) {
    static const std::unordered_map<std::string, TextureProperty> propertyMap = {
        {"texcolor", TextureProperty::TextureColor},
        {"parcolor", TextureProperty::ParticleColor},
        {"file", TextureProperty::FileName}
    };

    if (const auto texIt = propertyMap.find(key); texIt != propertyMap.end()) {
        return texIt->second;
    }

    return TextureProperty::Unknown;
}

AnimationProperty getAnimPropertyFromString(const std::string &key) {
    static const std::unordered_map<std::string, AnimationProperty> animPropertyMap = {
        {"xoffset", AnimationProperty::XOffset},
        {"yoffset", AnimationProperty::YOffset},
        {"cols", AnimationProperty::Cols},
        {"rows", AnimationProperty::Rows},
        {"ticks", AnimationProperty::Ticks},
        {"frames", AnimationProperty::Frames},
        {"bidir", AnimationProperty::Bidir},
        {"playing", AnimationProperty::Playing},
        {"padding", AnimationProperty::Padding},
        {"pxw", AnimationProperty::PixelWidth},
        {"pxh", AnimationProperty::PixelHeight}
    };

    const auto it = animPropertyMap.find(key);
    return it != animPropertyMap.end() ? it->second : AnimationProperty::Unknown;
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
    : maxTexSize(0),
      supportedFormats{".png", ".jpg", ".jpeg", ".tif", ".tiff", ".webp", ".jxl", ".avif"} {
    paddleTextures.fill(nullptr);
    ballTextures.fill(nullptr);
    brickTextures.fill(nullptr);
    powerUpTextures.fill(nullptr);
    miscTextures.fill(nullptr);
    effectTextures.fill(nullptr);
    titleTextures.fill(nullptr);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
}

TextureManager::~TextureManager() {
    clearTheme();
}

bool TextureManager::load(const std::filesystem::path &pathName, texture &tex) const {
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

    if (TextureUtilities::SDL_FlipSurfaceVertical(tempSurface)) {
        SDL_Log("Error: could not invert surface for texture: %s", pathName.c_str());
        SDL_FreeSurface(tempSurface);
        return false;
    }

    if (!TextureUtilities::createGLTextureFromSurface(tempSurface, tex.textureProperties.id, true)) {
        SDL_FreeSurface(tempSurface);
        return false;
    }

    tex.animationProperties.pxw = tempSurface->w;
    tex.animationProperties.pxh = tempSurface->h;
    SDL_FreeSurface(tempSurface);

    return true;
}

bool TextureManager::readTextureProperties(
    const std::filesystem::path &pathName,
    TextureResource &texResource,
    SpriteSheetAnimationProperties &animProps) {
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


        try {
            // Prüfen, ob es eine TextureProperty ist
            TextureProperty texProperty = getPropertyFromString(key);

            if (texProperty != TextureProperty::Unknown) {
                // TextureResource-Properties verarbeiten
                switch (texProperty) {
                    case TextureProperty::TextureColor:
                        if (value.length() >= 8) {
                            constexpr float normalizer = 1.0f / 255.0f;
                            texResource.textureColor[0] =
                                    static_cast<float>(std::stoi(value.substr(0, 2), nullptr, 16)) * normalizer;
                            texResource.textureColor[1] =
                                    static_cast<float>(std::stoi(value.substr(2, 2), nullptr, 16)) * normalizer;
                            texResource.textureColor[2] =
                                    static_cast<float>(std::stoi(value.substr(4, 2), nullptr, 16)) * normalizer;
                            texResource.textureColor[3] =
                                    static_cast<float>(std::stoi(value.substr(6, 2), nullptr, 16)) * normalizer;
                        }
                        break;
                    case TextureProperty::ParticleColor:
                        if (value.length() >= 6) {
                            constexpr float normalizer = 1.0f / 255.0f;
                            texResource.particleColor[0] =
                                    static_cast<float>(std::stoi(value.substr(0, 2), nullptr, 16)) * normalizer;
                            texResource.particleColor[1] =
                                    static_cast<float>(std::stoi(value.substr(2, 2), nullptr, 16)) * normalizer;
                            texResource.particleColor[2] =
                                    static_cast<float>(std::stoi(value.substr(4, 2), nullptr, 16)) * normalizer;
                        }
                        break;
                    case TextureProperty::FileName:
                        texResource.fileName = value;
                        break;
                    case TextureProperty::Unknown:
                        SDL_Log("Warning: '%s' hat unknown property key: '%s' with value: '%s'",
                                pathName.c_str(), key.c_str(), value.c_str());
                        break;
                }
            } else {
                // AnimationProperty verarbeiten
                switch (AnimationProperty animProperty = getAnimPropertyFromString(key)) {
                    case AnimationProperty::XOffset:
                        animProps.xoffset = std::stof(value);
                        break;
                    case AnimationProperty::YOffset:
                        animProps.yoffset = std::stof(value);
                        break;
                    case AnimationProperty::Cols:
                        animProps.cols = std::stoi(value);
                        break;
                    case AnimationProperty::Rows:
                        animProps.rows = std::stoi(value);
                        break;
                    case AnimationProperty::Ticks:
                        animProps.ticks = std::stoi(value);
                        break;
                    case AnimationProperty::Frames:
                        animProps.frames = std::stoi(value);
                        break;
                    case AnimationProperty::Bidir:
                        animProps.bidir = std::stoi(value) != 0;
                        break;
                    case AnimationProperty::Playing:
                        animProps.playing = std::stoi(value) != 0;
                        break;
                    case AnimationProperty::Padding:
                        animProps.padding = std::stoi(value) != 0;
                        break;
                    case AnimationProperty::PixelWidth:
                        animProps.pxw = std::stof(value);
                        break;
                    case AnimationProperty::PixelHeight:
                        animProps.pxh = std::stof(value);
                        break;
                    case AnimationProperty::Unknown:
                        SDL_Log("Warning: '%s' hat unknown property key: '%s' with value: '%s'",
                                pathName.c_str(), key.c_str(), value.c_str());
                        break;
                }
            }
        } catch (const std::exception &e) {
            SDL_Log("Error parsing property '%s' in '%s': %s", key.c_str(), pathName.c_str(), e.what());
        }
    }
    return true;
}

bool TextureManager::setSpriteTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
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

bool TextureManager::setBackgroundTheme(const std::string &themeName) {
    if (currentBackgroundTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearBackgroundTheme();

    currentBackgroundTheme = themeName;

    if (!loadAllBackgrounds()) {
        SDL_Log("Fehler beim Laden der Backgrounds für Theme: %s", currentTheme.c_str());
        return false;
    }

    return true;
}

void TextureManager::clearTheme() {
    for (auto &val: textureCache | std::views::values) {
        if (val) {
            glDeleteTextures(1, &val->textureProperties.id);
        }
    }

    textureCache.clear();

    for (auto &tex: paddleTextures) tex = nullptr;
    for (auto &tex: ballTextures) tex = nullptr;
    for (auto &tex: brickTextures) tex = nullptr;
    for (auto &tex: powerUpTextures) tex = nullptr;
    for (auto &tex: miscTextures) tex = nullptr;
    for (auto &tex: effectTextures) tex = nullptr;
    for (auto &tex: titleTextures) tex = nullptr;
}

texture *TextureManager::loadAndCacheTexture(const std::string &path, const bool forceReload) {
    if (!forceReload && textureCache.contains(path)) {
        return textureCache[path].get();
    }

    auto newTexture = std::make_unique<texture>();

    const std::string fullPath = currentTheme + "/" + path;

    if (!loadTextureWithProperties(fullPath, *newTexture)) {
        SDL_Log("Fehler: Konnte Textur nicht laden: %s", fullPath.c_str());
        return nullptr;
    }

    // Insert Cache and return Pointer
    textureCache[path] = std::move(newTexture);
    return textureCache[path].get();
}

texture *TextureManager::getTexture(const std::string &texturePath, const bool forceReload) {
    return loadAndCacheTexture(texturePath, forceReload);
}

texture *TextureManager::getPaddleTexture(PaddleTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= paddleTextures.size()) return nullptr;
    return paddleTextures[index];
}

texture *TextureManager::getBallTexture(BallTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= ballTextures.size()) return nullptr;
    return ballTextures[index];
}

texture *TextureManager::getBrickTexture(BrickTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= brickTextures.size()) return nullptr;
    return brickTextures[index];
}

texture *TextureManager::getPowerUpTexture(PowerUpTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= powerUpTextures.size()) return nullptr;
    return powerUpTextures[index];
}

texture *TextureManager::getEffectTexture(EffectTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= effectTextures.size()) return nullptr;
    return effectTextures[index];
}

texture *TextureManager::getMiscTexture(MiscTexture type) const {
    const auto index = static_cast<size_t>(type);
    if (index >= miscTextures.size()) return nullptr;
    return miscTextures[index];
}

bool TextureManager::loadAllGameTextures() {
    bool allTexturesLoaded = true;

    // Paddle-Texturen laden
    if (!((paddleTextures[static_cast<size_t>(PaddleTexture::Base)] = loadAndCacheTexture("gfx/paddle/base"))))
        allTexturesLoaded = false;
    if (!((paddleTextures[static_cast<size_t>(PaddleTexture::Glue)] = loadAndCacheTexture("gfx/paddle/glue"))))
        allTexturesLoaded = false;
    if (!((paddleTextures[static_cast<size_t>(PaddleTexture::Gun)] = loadAndCacheTexture("gfx/paddle/gun"))))
        allTexturesLoaded = false;

    // Ball-Texturen laden
    if (!((ballTextures[static_cast<size_t>(BallTexture::Normal)] = loadAndCacheTexture("gfx/ball/normal"))))
        allTexturesLoaded = false;
    if (!((ballTextures[static_cast<size_t>(BallTexture::Fireball)] = loadAndCacheTexture("gfx/ball/fireball"))))
        allTexturesLoaded = false;

    // Brick-Texturen laden
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Explosive)] = loadAndCacheTexture("gfx/brick/explosive"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Base)] = loadAndCacheTexture("gfx/brick/base"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Cement)] = loadAndCacheTexture("gfx/brick/cement"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Doom)] = loadAndCacheTexture("gfx/brick/doom"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Glass)] = loadAndCacheTexture("gfx/brick/glass"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Invisible)] = loadAndCacheTexture("gfx/brick/invisible"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Blue)] = loadAndCacheTexture("gfx/brick/blue"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Yellow)] = loadAndCacheTexture("gfx/brick/yellow"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Green)] = loadAndCacheTexture("gfx/brick/green"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Grey)] = loadAndCacheTexture("gfx/brick/grey"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Purple)] = loadAndCacheTexture("gfx/brick/purple"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::White)] = loadAndCacheTexture("gfx/brick/white"))))
        allTexturesLoaded = false;
    if (!((brickTextures[static_cast<size_t>(BrickTexture::Red)] = loadAndCacheTexture("gfx/brick/red"))))
        allTexturesLoaded = false;

    // Powerup-Texturen laden
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Coin)] = loadAndCacheTexture("gfx/powerup/coin"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Glue)] = loadAndCacheTexture("gfx/powerup/glue"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Multiball)] =
           loadAndCacheTexture("gfx/powerup/multiball"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::BigBall)] = loadAndCacheTexture("gfx/powerup/bigball"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::NormalBall)] = loadAndCacheTexture(
               "gfx/powerup/normalball"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::SmallBall)] =
           loadAndCacheTexture("gfx/powerup/smallball"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Aim)] = loadAndCacheTexture("gfx/powerup/aim"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Explosive)] =
           loadAndCacheTexture("gfx/powerup/explosive"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Gun)] = loadAndCacheTexture("gfx/powerup/gun"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Laser)] = loadAndCacheTexture("gfx/powerup/laser"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Life)] = loadAndCacheTexture("gfx/powerup/life"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Die)] = loadAndCacheTexture("gfx/powerup/die"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Drop)] = loadAndCacheTexture("gfx/powerup/drop"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Detonate)] =
           loadAndCacheTexture("gfx/powerup/detonate"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::ExplosiveGrow)] = loadAndCacheTexture(
               "gfx/powerup/explosive-grow"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::EasyBrick)] =
           loadAndCacheTexture("gfx/powerup/easybrick"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::NextLevel)] =
           loadAndCacheTexture("gfx/powerup/nextlevel"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::AimHelp)] = loadAndCacheTexture("gfx/powerup/aimhelp"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::GrowPaddle)] =
           loadAndCacheTexture("gfx/powerup/GrowPaddle"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::ShrinkPaddle)] = loadAndCacheTexture(
               "gfx/powerup/ShrinkPaddle"))))
        allTexturesLoaded = false;
    if (!((powerUpTextures[static_cast<size_t>(PowerUpTexture::Bullet)] = loadAndCacheTexture("gfx/powerup/bullet"))))
        allTexturesLoaded = false;

    // Misc Textures
    if (!((miscTextures[static_cast<size_t>(MiscTexture::Border)] = loadAndCacheTexture("gfx/border"))))
        allTexturesLoaded = false;

    // Effects Textures
    if (!((effectTextures[static_cast<size_t>(EffectTexture::Tail)] = loadAndCacheTexture("gfx/effects/tail"))))
        allTexturesLoaded = false;
    if (!((effectTextures[static_cast<size_t>(EffectTexture::Particle)] = loadAndCacheTexture("gfx/effects/particle"))))
        allTexturesLoaded = false;

    // Title Textures
    if (!((titleTextures[static_cast<size_t>(TitleTexture::Title)] = loadAndCacheTexture("gfx/title/title"))))
        allTexturesLoaded = false;

    return allTexturesLoaded;
}

bool TextureManager::loadTextureWithProperties(const std::string &basePath, texture &tex) const {
    // 1. load properties
    const std::filesystem::path propsPath = basePath + ".txt";

    bool hasProps = false;
    bool textureLoaded = false;
    std::filesystem::path actualImagePath;

    if (std::filesystem::exists(propsPath)) {
        hasProps = true;
        if (!readTextureProperties(propsPath, tex.textureProperties, tex.animationProperties)) {
            hasProps = false;
            SDL_Log("Warning: no properties '%s'", propsPath.c_str());
        }

        // 2. get image from textureProperties.fileName
        if (!tex.textureProperties.fileName.empty()) {
            const std::filesystem::path basePath_path(basePath);
            const std::filesystem::path themeRoot = basePath_path.parent_path().parent_path();
            actualImagePath = themeRoot / tex.textureProperties.fileName;

            if (std::filesystem::exists(actualImagePath)) {
                if (load(actualImagePath, tex)) {
                    textureLoaded = true;
                } else {
                    SDL_Log("Error: the image in the property couldn't be read: '%s'",
                            actualImagePath.c_str());
                }
            }
        }
    }

    // 3. try to get image from basename, case: missing properties file or no file specified
    if (!textureLoaded) {
        for (const auto &format: supportedFormats) {
            if (std::filesystem::path texturePath = basePath + format; std::filesystem::exists(texturePath)) {
                if (load(texturePath, tex)) {
                    textureLoaded = true;
                    actualImagePath = texturePath;
                    break;
                }
            }
        }
    }

    // 4. Fehlerbehandlung nach den genannten Regeln
    if (!textureLoaded) {
        if (hasProps && !tex.textureProperties.fileName.empty()) {
            SDL_Log("Error: Die in der Eigenschaftsdatei angegebene Bilddatei existiert nicht: '%s'",
                    tex.textureProperties.fileName.c_str());
        } else if (hasProps) {
            SDL_Log("Error: Eigenschaftsdatei gefunden, aber keine 'file=' Angabe oder die Datei existiert nicht: '%s'",
                    propsPath.c_str());
        } else {
            SDL_Log("Error: Keine Bilddatei für '%s' in unterstützten Formaten gefunden", basePath.c_str());
        }
        return false;
    }

    // 5. Standard-Eigenschaften setzen, wenn keine Eigenschaften geladen wurden
    if (!hasProps) {
        SDL_Log("Warning: no properties '%s'", actualImagePath.c_str());

        // Standardwerte für Textureigenschaften setzen
        tex.animationProperties.ticks = 1000;
        tex.animationProperties.cols = 1;
        tex.animationProperties.rows = 1;
        tex.animationProperties.xoffset = 1.0f;
        tex.animationProperties.yoffset = 1.0f;
        tex.animationProperties.frames = 1;
        tex.animationProperties.playing = false;
        tex.animationProperties.padding = false;

        // Standardfarben setzen (weiß)
        for (int i = 0; i < 3; i++) {
            tex.textureProperties.textureColor[i] = 1.0f;
            tex.textureProperties.particleColor[i] = 1.0f;
        }
        tex.textureProperties.textureColor[3] = 1.0f;
    }

    // 6. check colors and set fallback if not set
    bool hasColor = false;
    for (int i = 0; i < 4; i++) {
        if (tex.textureProperties.textureColor[i] > 0.0f) {
            hasColor = true;
            break;
        }
    }
    if (!hasColor) {
        SDL_Log("Warning: No texture color set in '%s', using white", basePath.c_str());
        for (int i = 0; i < 3; i++) {
            tex.textureProperties.textureColor[i] = 1.0f;
            tex.textureProperties.particleColor[i] = 1.0f;
        }
        tex.textureProperties.textureColor[3] = 1.0f;
    }
    return true;
}

bool TextureManager::loadAllBackgrounds() {
    const std::string directory = currentBackgroundTheme + "/gfx/bg/";

    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
        SDL_Log("Error: Background path '%s' does not exist or is not a directory", directory.c_str());
        return false;
    }
    clearBackgroundTheme();

    // Dateien einsammeln und nach Basename sortieren
    std::vector<std::filesystem::directory_entry> entries;
    for (const auto &entry: std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            if (const std::string ext = entry.path().extension().string();
                std::ranges::find(supportedFormats, ext) != supportedFormats.end()) {
                entries.push_back(entry);
            }
        }
    }
    // Natürliche Sortierung (natural sort)
    std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
        std::string nameA = a.path().stem().string();
        std::string nameB = b.path().stem().string();

        // Zu Kleinbuchstaben konvertieren für case-insensitive Sortierung
        std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);

        auto compareChunks = [](const std::string &a, const std::string &b) {
            if (std::isdigit(a[0]) && std::isdigit(b[0])) {
                // Numerischer Vergleich
                return std::stoi(a) < std::stoi(b);
            }
            // Alphabetischer Vergleich
            return a < b;
        };

        size_t posA = 0, posB = 0;
        while (posA < nameA.length() && posB < nameB.length()) {
            std::string chunkA, chunkB;

            // Extrahiere numerische oder alphabetische Chunks
            if (std::isdigit(nameA[posA]) && std::isdigit(nameB[posB])) {
                // Sammle Ziffern
                while (posA < nameA.length() && std::isdigit(nameA[posA])) {
                    chunkA += nameA[posA++];
                }
                while (posB < nameB.length() && std::isdigit(nameB[posB])) {
                    chunkB += nameB[posB++];
                }
            } else {
                // Sammle Buchstaben
                while (posA < nameA.length() && !std::isdigit(nameA[posA])) {
                    chunkA += nameA[posA++];
                }
                while (posB < nameB.length() && !std::isdigit(nameB[posB])) {
                    chunkB += nameB[posB++];
                }
            }

            if (chunkA != chunkB) {
                return compareChunks(chunkA, chunkB);
            }
        }

        return nameA.length() < nameB.length();
    });
#if DEBUG_SORT_BACKGROUNDS
    // Debug-Ausgabe der sortierten Liste
    SDL_Log("Sorted background files in '%s':", directory.c_str());
    for (size_t i = 0; i < entries.size(); ++i) {
        SDL_Log("[%zu] %s", i, entries[i].path().filename().string().c_str());
    }
#endif
    for (const auto &entry: entries) {
        const std::string filePath = entry.path().string();
        if (!backgroundCache.contains(filePath)) {
            auto tex = std::make_unique<texture>();
            if (load(filePath, *tex)) {
                backgroundTextures.push_back(tex.get());
                backgroundCache[filePath] = std::move(tex);
            } else {
                SDL_Log("Error: Failed to load background texture '%s'", filePath.c_str());
            }
        } else {
            backgroundTextures.push_back(backgroundCache[filePath].get());
        }
    }

    return !backgroundTextures.empty();
}

void TextureManager::clearBackgroundTheme() {
    for (auto &val: backgroundCache | std::views::values) {
        if (val) {
            glDeleteTextures(1, &val->textureProperties.id);
        }
    }
    backgroundTextures.clear();
    backgroundCache.clear();
}

texture *TextureManager::getBackground(const size_t index) const {
    if (index < backgroundTextures.size()) {
        return backgroundTextures[index];
    }
    return nullptr;
}
