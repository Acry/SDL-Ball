#pragma once
#include "texture.h"

class TextureManager;
class Background {
    GLuint displayList;
    GLfloat r[4], g[4], b[4], a;
    texture tex;
    int currentBgNumber;

    void drawQuad() const;

public:
    Background();
    ~Background();

    bool updateBgIfNeeded(TextureManager &texMgr);
    void init(TextureManager &texMgr);
    void draw() const;
};
