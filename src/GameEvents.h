// GameEvents.h
#pragma once
#include <functional>
#include <map>
#include <SDL2/SDL_keycode.h>

#include "BrickTypes.h"
#include "colors.h"
#include "GameObject.h"
#include "ICollideable.h"
#include "PowerupTypes.h"

enum class GameEvent {
    BallHitLeftBorder, // Index 0
    BallHitRightBorder, // Index 1
    BallHitTopBorder, // Index 2
    BallLost, // Index 3
    BallHitPaddle, // Index 4
    BrickDestroyed, // Index 5
    PowerUpCollected, // Index 6
    BallHitBrick, // Index 7
    PaddleHitLeftBorder, // Index 8
    PaddleHitRightBorder, // Index 9

    BallCreated,
    BallMoved,
    BallDestroyed,

    CreateTracer,
    TracerCreated,
    UpdateTracer,
    UpdateTracerColor,
    UpdateTracerSize,
    RemoveTracer,

    PaddleCreated,
    PaddleMoved,
    PaddleDestroyed,

    LevelRequested,
    LevelChanged,
    LevelThemeRequested,
    LevelThemeChanged,
    LevelLoaded,
    LevelStarted,

    KeyPressed,
    KeyReleased,
    MouseMoved,
    MouseCoordinatesNormalized,
    MouseButtonPressed,
    MouseButtonReleased,
    WindowResized,
    QuitRequested,

    ViewportResized,
};

struct ViewportEventData {
    int viewportX{0};
    int viewportY{0};
    int viewportW{0};
    int viewportH{0};
};

struct WindowEventData {
    int width{0};
    int height{0};
};

struct MouseCoordinatesNormalizedEventData {
    float x{0.0f};
    float y{0.0f};
};

struct MouseEventData {
    float x{0.0f};
    float y{0.0f};
};

struct KeyboardEventData {
    SDL_Keycode key{0};
    bool isPressed{false};
};

struct BrickInfo {
    BrickType type{BrickType::None};
    float x{0.0f};
    float y{0.0f};
    Color CustomBrickColor{};
};

struct PowerupData {
    std::map<size_t, PowerupType> powerupMap{}; // Index -> PowerUp-Typ
};

struct LevelData {
    std::string themeName; // Name des aktuellen Themes
    std::vector<BrickInfo> bricks; // Alle Bricks des Levels
    PowerupData powerupData; // PowerUp-Daten für das Level
    bool isDropping{false}; // Ob das Level ein "dropping level" ist
    int dropSpeed{0}; // Geschwindigkeit des Drops (wenn isDropping true ist)
    size_t currentLevel{0};
};

struct CollisionData {
    const ICollideable *object1{nullptr};
    const ICollideable *object2{nullptr};
    float hitX{0.0f};
    float hitY{0.0f};
};

struct EventData {
    float posX{0};
    float posY{0};
    int soundID{-1};
    GameObject *sender{nullptr};
    const GameObject *target{nullptr};
    // ---snip--- TODO/FIXME
    int points{0}; // to BrickManager: brick destroyed data
};

struct SubThemeData {
    std::string subThemeName; // Name des Unterthemas, z.B. "default",
    std::string subThemePath; // Pfad zum Sprite-Untertitel
};

struct ThemeData {
    SubThemeData spritesTheme; // Pfad zum Sprite-Theme
    SubThemeData fontsTheme; // Pfad zum Font-Theme
    SubThemeData levelsTheme; // Pfad zum Level-Theme
    SubThemeData backgroundTheme; // Pfad zum BG-Theme
    SubThemeData soundTheme; // Pfad zum Sound-Theme
};

struct LevelRequestedData {
    size_t level{0}; // Angeforderte Level-Nummer
};

struct LevelThemeData {
    size_t maxLevel{};
};

using EventCallback = std::function<void(const EventData &)>;
using LevelEventCallback = std::function<void(const LevelData &)>;
using LevelThemeEventCallback = std::function<void(const LevelThemeData &)>;
using LevelRequestedEventCallback = std::function<void(const LevelRequestedData &)>;
using ThemeEventCallback = std::function<void(const ThemeData &)>;
using CollisionEventCallback = std::function<void(const CollisionData &)>;

// Input event callbacks
using KeyboardEventCallback = std::function<void(const KeyboardEventData &)>;
using MouseEventCallback = std::function<void(const MouseEventData &)>;
using MouseCoordinatesNormalizedEventCallback = std::function<void(const MouseCoordinatesNormalizedEventData &)>;

using WindowEventCallback = std::function<void(const WindowEventData &)>;
using ViewportEventCallback = std::function<void(const ViewportEventData &)>;
