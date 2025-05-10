#pragma once

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

struct glCharInfo_struct {
    GLfloat Xa, Ya, Xb, Yb, width;
};

struct glFontInfo_struct {
    GLuint tex;
    GLfloat height;
    glCharInfo_struct ch[255];
};

class Text {
    static Text* instance;
    Text();  // Konstruktor private
public:
    Text(const Text&) = delete;
    void operator=(const Text&) = delete;

    static Text& getInstance() {
        if (instance == nullptr) {
            instance = new Text();
        }
        return *instance;
    }
    ~Text();
    GLfloat getHeight(int font) const;
    void write(const std::string &text, int font, bool center, GLfloat scale, GLfloat x, GLfloat y) const;

private:
    void genFontTex(const std::string &TTFfontName, int fontSize, int font);
    glFontInfo_struct fontInfo[FONT_NUM];
};
