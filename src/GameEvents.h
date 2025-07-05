// GameEvents.h
#pragma once
#include <functional>
#include <map>
#include <string>
#include <SDL2/SDL_keycode.h>

#include "BrickTypes.h"
#include "GameObject.h"
#include "ICollideable.h"
#include "PowerupTypes.h"
#include "colors.h"

enum class GameEvent {
    // Collision events
    BallHitLeftBorder,
    BallHitRightBorder,
    BallHitTopBorder,
    BallHitBall,
    BallLost,
    BallHitPaddle,
    BallHitBrick,

    PowerUpCollected,
    PaddleHitLeftBorder,
    PaddleHitRightBorder,

    BallCreated,
    BallMoved,
    BallDestroyed,
    LastBallDestroyed, // BallManager: Last ball destroyed event

    CreateTracer,
    TracerCreated,
    UpdateTracer,
    UpdateTracerColor,
    UpdateTracerSize,
    RemoveTracer,

    PaddleCreated, // PaddleManager: Paddle created event
    PaddleMoved, // PaddleManager: Paddle moved event
    PaddleDestroyed, // PaddleManager: Paddle destroyed event

    LevelRequested,
    LevelChanged,

    LevelThemeRequested,
    LevelThemeChanged,

    LevelLoaded, // LevelManager: Level loaded event
    LevelStarted, // GameManager: Level started event

    FontThemeRequested, // TextManager: Font theme requested event
    FontThemeChanged, // TextManager: Font theme changed event

    ThemeRequested, // ThemeManager: Theme requested event
    ThemeChanged, // ThemeManager: Theme changed event

    SettingsLoadRequested,
    SettingsLoaded, // SettingsManager: Settings loaded event

    SettingsSaveRequested,
    SettingsSaved, // SettingsManager: Settings saved event

    BrickDestroyed, // BrickManager: Brick destroyed event
    LevelCleared, // BrickManager: Level cleared event

    DifficultyChanged, // MenuManager: Difficulty changed event

    ScoreChanged,

    KeyPressed,
    KeyReleased,
    KeyboardPaddleMove,

    MouseButtonPressed, // MouseManager internal
    MouseButtonPressedNormalized, // MouseManager external
    MouseButtonReleased, // MouseManager internal
    MouseButtonReleasedNormalized, // MouseManager external
    MouseCoordinatesNormalized, // MouseManager external
    MouseMoved, // MouseManager internal
    MouseWheelScrolled, // MouseManager internal
    MouseWheelScrolledNormalized, // MouseManager external

    WindowResized,
    QuitRequested,

    MenuKeyPressed,
    MenuKeyReleased, // MenuManager: Key pressed in menu
    MenuOpened,
    MenuClosed,
    SceneChangedRequested,
    SceneChanged,
    ScreenshotRequested,
    ScreenshotCreated,
    ViewportResized,

    None,
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
    uint8_t button{0};
    int wheelX{0}; // Horizontales Scrollen
    int wheelY{0}; // Vertikales Scrollen
    bool wheelFlipped{false}; // Ob die Scroll-Richtung umgedreht ist
};

struct KeyboardEventData {
    SDL_Keycode key{0};
    bool isPressed{false};
};

struct KeyboardMoveEventData {
    float direction; // -1.0 für links, 0.0 für keine Bewegung, 1.0 für rechts
    float speed; // Bewegungsgeschwindigkeit für die Tastatursteuerung
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
using KeyboardMoveEventCallback = std::function<void(const KeyboardMoveEventData &)>;
using MouseEventCallback = std::function<void(const MouseEventData &)>;
using MouseCoordinatesNormalizedEventCallback = std::function<void(const MouseCoordinatesNormalizedEventData &)>;

using WindowEventCallback = std::function<void(const WindowEventData &)>;
using ViewportEventCallback = std::function<void(const ViewportEventData &)>;
