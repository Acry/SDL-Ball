// TextureManager.h
#pragma once

#include <filesystem>
#include <string>

#include "SpriteSheetAnimation.h"
#include "ThemeManager.h"

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
    std::unordered_map<std::string, std::unique_ptr<SpriteSheetAnimation> > textureCache;

    std::array<SpriteSheetAnimation *, static_cast<size_t>(PaddleTexture::Count)> paddleTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(BallTexture::Count)> ballTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(BrickTexture::Count)> brickTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(PowerUpTexture::Count)> powerUpTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(MiscTexture::Count)> miscTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(EffectTexture::Count)> effectTextures{};
    std::array<SpriteSheetAnimation *, static_cast<size_t>(TitleTexture::Count)> titleTextures{};

    int maxTexSize;
    std::string currentTheme;

    SpriteSheetAnimation *loadAndCacheTexture(const std::string &path, bool forceReload = false);

    // create texture from image
public:
    TextureManager();

    ~TextureManager();

    TextureManager(const TextureManager &) = delete;

    TextureManager &operator=(const TextureManager &) = delete;

    bool setSpriteTheme(const std::string &themeName);

    void clearTheme();

    SpriteSheetAnimation *getTexture(const std::string &texturePath, bool forceReload = false);

    SpriteSheetAnimation *getPaddleTexture(PaddleTexture type) const;

    SpriteSheetAnimation *getBallTexture(BallTexture type) const;

    SpriteSheetAnimation *getBrickTexture(BrickTexture type) const;

    SpriteSheetAnimation *getPowerUpTexture(PowerUpTexture type) const;

    SpriteSheetAnimation *getEffectTexture(EffectTexture type) const;

    SpriteSheetAnimation *getMiscTexture(MiscTexture type) const;

    // LoadAllGameTexturesWithProperties
    bool loadAllGameTextures();

    // Load SpriteTextures
    // Load BackgroundTextures
    // Load MenuTextures
    // Load EffectsTestures

    // Load image file
    bool load(const std::filesystem::path &pathName, SpriteSheetAnimation &tex) const;

    // load property file
    static bool readTextureProperties(const std::filesystem::path &pathName, SpriteSheetAnimation &tex);

    bool loadTextureWithProperties(const std::string &basePath, SpriteSheetAnimation &animation) const;
};
