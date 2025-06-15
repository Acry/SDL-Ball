// TextureManager.h
#pragma once

#include <epoxy/gl.h>
#include <filesystem>
#include <string>

#include "SpriteSheetAnimation.h"
#include "ThemeManager.h"

struct TextureResource {
    GLuint id{0}; // OpenGL Textur-ID
    GLfloat textureColor[4]{1.0f, 1.0f, 1.0f, 1.0f}; // RGBA-Farbmodifikator
    GLfloat particleColor[3]{1.0f, 1.0f, 1.0f}; // RGB-Partikelfarbe
    std::string fileName; // Pfad zur Textur
};

enum class TextureProperty {
    TextureId, // OpenGL Textur-ID
    TextureColor, // Für textureColor[4]
    ParticleColor, // Für particleColor[3]
    FileName, // Für Dateinamen
    Unknown
};

struct texture {
    TextureResource textureProperties; // Textur-Eigenschaften
    SpriteSheetAnimationProperties animationProperties{}; // Animationseigenschaften
};

// HINT: in game_state.h sind noch Textur-Eigenschaften definiert.

enum class PaddleTexture {
    Base,
    Glue,
    Gun,
    Count
};

enum class BallTexture {
    Normal,
    Fireball,
    Count
};

enum class BrickTexture {
    Base,
    Blue,
    Cement,
    Doom,
    Explosive,
    Glass,
    Green,
    Grey,
    Invisible,
    Purple,
    Red,
    White,
    Yellow,
    Count
};

enum class PowerUpTexture {
    Aim,
    AimHelp,
    BigBall,
    Bullet,
    Coin,
    Detonate,
    Die,
    Drop,
    EasyBrick,
    Explosive,
    ExplosiveGrow,
    Glue,
    GoThrough,
    GrowPaddle,
    Gun,
    Laser,
    Life,
    Multiball,
    NextLevel,
    NormalBall,
    ShrinkPaddle,
    SmallBall,
    Count
};

enum class MiscTexture {
    Border,
    Count
};

enum class EffectTexture {
    Tail,
    Particle,
    Count
};

enum class TitleTexture {
    Title,
    Count
};

class TextureManager {
    // caches
    std::unordered_map<std::string, std::unique_ptr<texture> > textureCache;
    std::unordered_map<std::string, std::unique_ptr<texture> > backgroundCache;

    std::array<texture *, static_cast<size_t>(PaddleTexture::Count)> paddleTextures{};
    std::array<texture *, static_cast<size_t>(BallTexture::Count)> ballTextures{};
    std::array<texture *, static_cast<size_t>(BrickTexture::Count)> brickTextures{};
    std::array<texture *, static_cast<size_t>(PowerUpTexture::Count)> powerUpTextures{};
    std::array<texture *, static_cast<size_t>(MiscTexture::Count)> miscTextures{};
    std::array<texture *, static_cast<size_t>(EffectTexture::Count)> effectTextures{};
    std::array<texture *, static_cast<size_t>(TitleTexture::Count)> titleTextures{};

    std::vector<texture *> backgroundTextures;

    int maxTexSize;
    std::string currentTheme;
    std::string currentBackgroundTheme;
    const std::vector<std::string> supportedFormats;

    texture *loadAndCacheTexture(const std::string &path, bool forceReload = false);

public:
    TextureManager();

    ~TextureManager();

    TextureManager(const TextureManager &) = delete;

    TextureManager &operator=(const TextureManager &) = delete;

    bool setSpriteTheme(const std::string &themeName);

    bool setBackgroundTheme(const std::string &themeName);

    bool loadAllBackgrounds();

    size_t getBackgroundCount() const { return backgroundTextures.size(); }

    texture *getBackground(const size_t index) const;

    void clearTheme();

    void clearBackgroundTheme();

    texture *getTexture(const std::string &texturePath, bool forceReload = false);

    texture *getPaddleTexture(PaddleTexture type) const;

    texture *getBallTexture(BallTexture type) const;

    texture *getBrickTexture(BrickTexture type) const;

    texture *getPowerUpTexture(PowerUpTexture type) const;

    texture *getEffectTexture(EffectTexture type) const;

    texture *getMiscTexture(MiscTexture type) const;

    // LoadAllGameTexturesWithProperties
    bool loadAllGameTextures();

    // Load SpriteTextures
    // Load BackgroundTextures
    // Load MenuTextures
    // Load EffectsTextures

    // Load image file
    bool load(const std::filesystem::path &pathName, texture &tex) const;

    static bool readTextureProperties(const std::filesystem::path &pathName, TextureResource &texResource,
                                      SpriteSheetAnimationProperties &animProps);

    bool loadTextureWithProperties(const std::string &basePath, texture &animation) const;

    const std::unordered_map<std::string, std::unique_ptr<texture> > &getTextureCache() const {
        return textureCache;
    }
};
