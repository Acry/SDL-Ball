// sound.h
#pragma once
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <epoxy/gl.h>
#include "config.h"

struct sampleQueuedItem {
    int s, p, num;
};

class soundClass {
    Mix_Chunk *sample[USED_SOUND_SAMPLES];
    std::vector<sampleQueuedItem> q;

    void loadSample(const char *SampleName, int sampleNum);

    int currentChannels = 0;
    int breakSoundIndex = 0;

public:
    bool init();
    void play();
    void add(int i, GLfloat x);
    void loadSounds();
    ~soundClass();
};