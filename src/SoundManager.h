// sound.h
#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <epoxy/gl.h>
#include "config.h"

struct sampleQueuedItem {
    int s, p, num;
};

class SoundManager {
    Mix_Chunk *sample[USED_SOUND_SAMPLES];
    std::vector<sampleQueuedItem> q;

    bool loadSample(const char *SampleName, int sampleNum);

    std::string soundThemePath;
    int currentChannels = 0;
    int breakSoundIndex = 0;

public:
    SoundManager();

    bool init();

    void play();

    void queueSound(int i, GLfloat x);

    bool setSoundTheme(const std::string &fontFilePath);

    void clearSoundTheme();

    ~SoundManager();
};
