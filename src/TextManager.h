// TextManager.h
#pragma once

#include <list>
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

private:
    bool genFontTex(const std::string &TTFfontName, int fontSize, Fonts font);

    void clearTheme();

    glFontInfo_struct fontInfo[static_cast<int>(Fonts::Count)];
    std::string themePath;
    std::list<TextAnnouncement> announcements;
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
