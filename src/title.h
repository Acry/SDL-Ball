// title.h
#pragma once
#include <string>
#include "textureClass.h"
#include "moving_object.h"
#include "effectManager.h"
#include "menuClass.h"

constexpr int MAXPOTEXTURES = 21;

class powerupDescriptionClass : public moving_object {
public:
    powerupDescriptionClass();

    textureClass *tex;
    void draw() const;

    std::string name;
    std::string description;
};

class titleScreenClass {
    effectManager *fxMan;
    int ticksSinceLastSpawn;
    textureManager texMgr;
    textureClass texTitle;
    textureClass *texPowerups;
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
    titleScreenClass(effectManager *m, textureClass tp[], menuClass *me);
    void draw(Uint32 *frame_age, Uint32 *max_frame_age);
};