#pragma once
#include "Texture.h"

class TextureManager;
class BackgroundManager {
    GLuint backgroundDisplayList;
    GLfloat r[4], g[4], b[4], a;
    Texture tex;
    int currentBgNumber;

    void drawQuad() const;

public:
    BackgroundManager();
    ~BackgroundManager();

    bool updateBgIfNeeded(const TextureManager &texMgr);
    void init(const TextureManager &texMgr);
    void draw() const;
};
