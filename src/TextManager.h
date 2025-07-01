// TextManager.h
#pragma once

#include <epoxy/gl.h>
#include <SDL2/SDL_stdinc.h>
#include <list>
#include <map>
#include <string>

#include "GameEvents.h"

class IEventManager;

// PRINTABLE_CHARS =  96; // 128 - 32 = 96 characters
constexpr Uint32 CHARS = 128; // 0-127 ASCII characters
enum class Fonts {
    Menu,
    AnnounceGood,
    AnnounceBad,
    Highscore,
    MenuHighscore,
    IntroHighscore,
    IntroDescription,
    Count
};

class TextAnnouncement;

struct characterUvCoordinates {
    GLfloat uLeft, vTop, uRight, vBottom, width;
};

struct fontInfo {
    GLuint texture;
    GLfloat height; // Maximalhöhe der Schriftart in OpenGL-normalisierten Werten (0.0-1.0)
    characterUvCoordinates uv[CHARS];
    int ascent; // Obere Höhe über der Baseline
    int descent; // Untere Höhe unter der Baseline
    int lineSkip; // Empfohlener Zeilenabstand
};

class TextManager {
    IEventManager *eventManager;
    std::string currentTheme;
    ThemeData currentThemeData;

    bool loadAllFonts();

    bool genFontTex(const std::string &TTFfontName, int fontSize, Fonts font);

    static Fonts getFontTypeFromKey(const std::string &key);

    static bool parseFontThemeDescriptionFile(const std::string &filePath,
                                              std::map<std::string, std::string> &fontFiles,
                                              std::map<std::string, int> &fontSizes);

    bool createFontTextures(const std::map<std::string, std::string> &fontFiles,
                            const std::map<std::string, int> &fontSizes);

    void clearTheme();

    fontInfo fontInfo[static_cast<int>(Fonts::Count)];
    std::list<TextAnnouncement> announcements;

public:
    explicit TextManager(IEventManager *evtMgr);

    bool init();

    ~TextManager();

    bool setTheme(const std::string &fontFilePath);

    GLfloat getHeight(Fonts font) const;

    void write(const std::string &text, Fonts font, bool center, GLfloat scale, GLfloat x, GLfloat y) const;

    void addAnnouncement(const std::string &message, int lifetime, Fonts font);

    void updateAnnouncements(const float deltaTime);

    void drawAnnouncements(const float deltaTime) const;

    size_t getAnnouncementCount() const;

    void handleFontThemeRequested(const ThemeData &data);
};

class TextAnnouncement {
    int age;
    float zoom, fade;
    bool fadingOut;
    bool active;
    std::string message;
    Fonts font;
    int lifetime;
    TextManager *textManager;

public:
    TextAnnouncement(std::string msg, Fonts fontId, int ttl, TextManager *mgr);

    void update(const float deltaTime);

    void draw(const float deltaTime) const;

    bool isActive() const { return active; }
};
