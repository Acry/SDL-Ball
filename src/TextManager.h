// TextManager.h
#pragma once

#include <list>
#include <map>
#include <string>
#include <epoxy/gl.h>

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

struct glCharInfo_struct {
    GLfloat Xa, Ya, Xb, Yb, width;
};

struct glFontInfo_struct {
    GLuint tex;
    GLfloat height;
    glCharInfo_struct ch[255];
};

class TextManager {
    std::string currentTheme;

    bool loadAllFonts();

    bool genFontTex(const std::string &TTFfontName, int fontSize, Fonts font);

    Fonts getFontTypeFromKey(const std::string& key);

    bool parseFontThemeDescriptionFile(const std::string& filePath, std::map<std::string, std::string>& fontFiles, std::map<std::string, int>& fontSizes);

    bool createFontTextures(const std::map<std::string, std::string>& fontFiles, const std::map<std::string, int>& fontSizes);
    void clearTheme();

    glFontInfo_struct fontInfo[static_cast<int>(Fonts::Count)];
    std::list<TextAnnouncement> announcements;

public:
    TextManager();

    ~TextManager();

    bool setTheme(const std::string &fontFilePath);

    GLfloat getHeight(Fonts font) const;

    void write(const std::string &text, Fonts font, bool center, GLfloat scale, GLfloat x, GLfloat y) const;

    void addAnnouncement(const std::string &message, int lifetime, Fonts font);

    void updateAnnouncements(const float deltaTime);

    void drawAnnouncements(const float deltaTime);

    size_t getAnnouncementCount() const;
};

class TextAnnouncement {
    int age;
    float zoom, fade;
    bool fadingOut;
    bool active;
    std::string message;
    Fonts font;
    int lifetime;
    TextManager *textManager; // Zeiger auf die übergeordnete TextManager-Instanz

public:
    TextAnnouncement(std::string msg, Fonts fontId, int ttl, TextManager *mgr);

    void update(const float deltaTime);

    void draw(const float deltaTime) const;

    bool isActive() const { return active; }
};
