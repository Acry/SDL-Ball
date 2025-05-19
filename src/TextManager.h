#pragma once

#include <list>
#include <string>
#include <epoxy/gl.h>
// Jeder Index entspricht einer bestimmten Font-Konfiguration aus der Datei fonts.txt.
#define FONT_MENU 0
#define FONT_ANNOUNCE_GOOD 1
#define FONT_ANNOUNCE_BAD 2
#define FONT_HIGHSCORE 3
#define FONT_MENUHIGHSCORE 4
#define FONT_INTROHIGHSCORE 5
#define FONT_INTRODESCRIPTION 6
#define FONT_NUM 7


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

    // Methode zum Setzen des Font-Themes (vollständiger Pfad zur fonts.txt)
    bool setFontTheme(const std::string& fontFilePath);

    GLfloat getHeight(int font) const;
    void write(const std::string &text, int font, bool center, GLfloat scale, GLfloat x, GLfloat y) const;
    void addAnnouncement(const std::string& message, int lifetime, int font);
    void updateAnnouncements(float deltaTime);
    void drawAnnouncements();

private:
    void genFontTex(const std::string &TTFfontName, int fontSize, int font);
    void clearFontTheme();

    glFontInfo_struct fontInfo[FONT_NUM];
    std::string fontThemePath;
    std::list<TextAnnouncement> announcements;
};

class TextAnnouncement {
    int age;
    float zoom, fade;
    bool fadingOut;
    bool active;
    std::string message;
    int font;
    int lifetime;
    TextManager* textManager; // Zeiger auf die übergeordnete TextManager-Instanz

public:
    TextAnnouncement(const std::string& msg, int fontId, int ttl, TextManager* mgr);

    void update(float deltaTime);
    void draw();
    bool isActive() const { return active; }
};
