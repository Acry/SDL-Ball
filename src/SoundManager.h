// sound.h
#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <epoxy/gl.h>
#include "config.h"

class EventManager;

struct sampleQueuedItem {
    int s{}; // Sound-Index
    int p{}; // Panning
    int volume{MIX_MAX_VOLUME}; // Lautstärke basierend auf y-Position
    int num{1}; // Anzahl der Wiederholungen
};

class SoundManager {
    std::string currentTheme;
    EventManager *eventManager = nullptr;
    Mix_Chunk *sample[USED_SOUND_SAMPLES];
    std::vector<sampleQueuedItem> q;

    bool loadAllSounds();
    bool loadSample(const std::string& fullSamplePath, const int sampleNum);

    int currentChannels = 0;
    int breakSoundIndex = 0;

public:
    SoundManager();

    bool init();

    void play();

    void queueSound(int i, GLfloat x, GLfloat y);

    bool setTheme(const std::string &themeName);

    void clearTheme();

    void registerEvents(EventManager *evManager);

    ~SoundManager();
};
