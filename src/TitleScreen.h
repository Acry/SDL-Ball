// title.h
#pragma once
#include <string>
#include "SpriteSheetAnimation.h"
#include "MovingObject.h"
#include "TextureManager.h""
#include "Menu.h"

constexpr int MAXPOTEXTURES = 21;

class powerupDescriptionClass : public MovingObject {
public:
    powerupDescriptionClass();

    SpriteSheetAnimation *tex;
    void draw() const;

    std::string name;
    std::string description;
};

class TitleScreen {
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    TextureManager texMgr;
    SpriteSheetAnimation texTitle;
    SpriteSheetAnimation *texPowerups;
    GLuint glTitleList;
    float rot;
    bool rotDir;
    powerupDescriptionClass powerUp[MAXPOTEXTURES];
    int numHighScores;
    pos runnerPos;
    menuClass *menu;
    int runnerTime;
    float runnerVelX, runnerVelY;
    int hilight;
    bool hilightDir;
    int hilightTime;

    static void readDescriptions(powerupDescriptionClass po[]);

public:
    TitleScreen(effectManager *m, SpriteSheetAnimation tp[], menuClass *me);
    void draw(Uint32 *frame_age, Uint32 *max_frame_age);
};