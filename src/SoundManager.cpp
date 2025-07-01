#include <vector>
#include <filesystem>
#include <SDL_log.h>
#include <string>
#include <SDL2/SDL_mixer.h>

#include "SoundManager.h"
#include "EventManager.h"

SoundManager::SoundManager() : currentTheme{}, sample{}, q{}, currentChannels{0}, breakSoundIndex{0} {
    // Array der Samples initialisieren
    for (auto &i: sample) {
        i = nullptr;
    }
    init();
}

// if init return false, settingsManager should set setting.sound to false
bool SoundManager::init() {
    // Initialize SDL2_mixer with support for common audio formats
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) < 0) {
        SDL_Log("Mix_Init failed: %s\n", Mix_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        SDL_Log("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return false;
    }
    if (currentChannels = Mix_AllocateChannels(-1); currentChannels < GAME_AUDIO_CHANNELS) {
        // wenn weniger Kanäle verfügbar sind als gewünscht,
        // behalte die verfügbare Anzahl bei
        Mix_AllocateChannels(currentChannels);
    } else {
        // wenn genügend Kanäle verfügbar sind,
        // alloziere die gewünschte Anzahl
        Mix_AllocateChannels(GAME_AUDIO_CHANNELS);
    }
    return true;
}

bool SoundManager::loadSample(const std::string &fullSamplePath, const int sampleNum) {
    sample[sampleNum] = Mix_LoadWAV(fullSamplePath.c_str());
    if (!sample[sampleNum]) {
        SDL_Log("SoundManager '%s' :%s", fullSamplePath.c_str(), Mix_GetError());
        return false;
    }
    return true;
}

void SoundManager::queueSound(const int i, const GLfloat x, const GLfloat y = 0.0f) {
    // Panning berechnen (links/rechts basierend auf x)
    const int p = (255.0 / 3.2f) * (x + 1.0);

    // Minimale Lautstärke (50% des Maximums)
    constexpr float MIN_VOLUME_PERCENTAGE = 0.5f;

    // Lautstärke mit Mindestpegel berechnen
    // y=-1.0 (Paddle): 100% Lautstärke
    // y=+1.0 (oberer Rand): 50% Lautstärke
    const float normalizedY = (y + 1.0f) / 2.0f;
    const int volume = MIX_MAX_VOLUME * (MIN_VOLUME_PERCENTAGE +
                                         (1.0f - MIN_VOLUME_PERCENTAGE) * (1.0f - normalizedY));

    sampleQueuedItem qt;
    qt.s = i; // Sound-Index
    qt.p = p; // Panning
    qt.volume = volume; // Neue Eigenschaft für Lautstärke
    q.push_back(qt);
}

bool SoundManager::setTheme(const std::string &themeName) {
    if (currentTheme == themeName) return true;
    if (!std::filesystem::exists(themeName)) {
        SDL_Log("Error: Could not read theme-directory: %s", themeName.c_str());
        return false;
    }
    clearTheme();
    currentTheme = themeName;

    if (!loadAllSounds()) {
        SDL_Log("Error getting sound theme: %s", currentTheme.c_str());
        return false;
    }
    return true;
}

void SoundManager::play() {
    // Playlist für zusammengefasste Sounds
    std::vector<sampleQueuedItem> pl;

    // Queue durchlaufen und gleiche Samples zusammenfassen
    for (auto &it: q) {
        bool same = false;
        for (auto &[s, p, volume, num]: pl) {
            if (s == it.s) {
                same = true;
                num++;
                p += it.p;
                volume += it.volume;
            }
        }

        // Sample noch nicht in der Playlist
        if (!same) {
            pl.push_back(it);
            pl.back().num = 1;
        }
    }
    q.clear();

    // Aktuelle Samples abspielen
    for (auto &item: pl) {
        int freeChannel = -1;

        // Freien Kanal suchen
        for (int i = 0; i < currentChannels; i++) {
            if (!Mix_Playing(i)) {
                freeChannel = i;
                break;
            }
        }

        // Kein freier Kanal gefunden
        if (freeChannel == -1) {
            SDL_Log("No free channel available for %i", item.s);
            continue;
        }

        // Soundvarianten für normale Brick-Breaks
        if (item.s == SND_NORM_BRICK_BREAK) {
            switch (breakSoundIndex) {
                case 1: item.s = SND_NORM_BRICK_BREAKB;
                    break;
                case 2: item.s = SND_NORM_BRICK_BREAKC;
                    break;
                case 3: item.s = SND_NORM_BRICK_BREAKD;
                    break;
                case 4: item.s = SND_NORM_BRICK_BREAKE;
                    break;
                default: ; // Case 0 = SND_NORM_BREAK(a)
            }

            breakSoundIndex = (breakSoundIndex + 1) % 5;
        }

        // Durchschnittswerte berechnen
        item.p /= item.num;
        item.volume /= item.num;

        // Audio-Eigenschaften setzen
        Mix_SetPanning(freeChannel, 255 - item.p, item.p);
        Mix_VolumeChunk(sample[item.s], item.volume);

        if (Mix_PlayChannel(freeChannel, sample[item.s], 0) == -1) {
            SDL_Log("Error: Sample %i: %s", item.s, Mix_GetError());
        }
    }
}

SoundManager::~SoundManager() {
    if (eventManager) {
        // Alle Event-Listener entfernen
        eventManager->removeListener(GameEvent::BallHitLeftBorder, this);
        eventManager->removeListener(GameEvent::BallHitRightBorder, this);
        eventManager->removeListener(GameEvent::BallHitTopBorder, this);
        eventManager->removeListener(GameEvent::BallHitPaddle, this);
        eventManager->removeListener(GameEvent::BrickDestroyed, this);
        // Weitere Events...
    }
    clearTheme();
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    Mix_CloseAudio();
    Mix_Quit();
}

void SoundManager::clearTheme() {
    for (const auto &i: sample) {
        Mix_FreeChunk(i);
    }
}

void SoundManager::registerEvents(EventManager *evManager) {
    if (!evManager) return;

    eventManager = evManager;

    // Kollisions-Events mit Sounds verknüpfen
    eventManager->addListener(GameEvent::BallHitLeftBorder, [this](const EventData &data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX, data.posY);
    }, this);

    eventManager->addListener(GameEvent::BallHitRightBorder, [this](const EventData &data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX, data.posY);
    }, this);

    eventManager->addListener(GameEvent::BallHitTopBorder, [this](const EventData &data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX, data.posY);
    }, this);

    eventManager->addListener(GameEvent::BallHitPaddle, [this](const EventData &data) {
        this->queueSound(SND_BALL_HIT_PADDLE, data.posX, data.posY);
    }, this);

    eventManager->addListener(GameEvent::BrickDestroyed, [this](const EventData &data) {
        this->queueSound(SND_NORM_BRICK_BREAK, data.posX, data.posY);
    }, this);

    // FIXME
    eventManager->addListener(GameEvent::BallHitBrick, [this](const EventData &data) {
        this->queueSound(SND_NORM_BRICK_BREAK, data.posX, data.posY);
    }, this);
    // Weitere Event-Listener hinzufügen...
}

bool SoundManager::loadAllSounds() {
    const std::string fullPath = currentTheme + "/snd/";

    bool allSamplesLoaded = true;

    // 25 Sounds → USED_SOUND_SAMPLES
    // Samples nacheinander laden und Fehler prüfen
    allSamplesLoaded &= loadSample(fullPath + "start.ogg", SND_START);
    allSamplesLoaded &= loadSample(fullPath + "ball-hit-border.ogg", SND_BALL_HIT_BORDER);
    allSamplesLoaded &= loadSample(fullPath + "ball-hit-paddle.ogg", SND_BALL_HIT_PADDLE);
    allSamplesLoaded &= loadSample(fullPath + "norm-brick-breaka.ogg", SND_NORM_BRICK_BREAK);
    allSamplesLoaded &= loadSample(fullPath + "norm-brick-breakb.ogg", SND_NORM_BRICK_BREAKB);
    allSamplesLoaded &= loadSample(fullPath + "norm-brick-breakc.ogg", SND_NORM_BRICK_BREAKC);
    allSamplesLoaded &= loadSample(fullPath + "norm-brick-breakd.ogg", SND_NORM_BRICK_BREAKD);
    allSamplesLoaded &= loadSample(fullPath + "norm-brick-breake.ogg", SND_NORM_BRICK_BREAKE);
    allSamplesLoaded &= loadSample(fullPath + "expl-brick-break.ogg", SND_EXPL_BRICK_BREAK);
    allSamplesLoaded &= loadSample(fullPath + "glass-brick-hit.ogg", SND_GLASS_BRICK_HIT);
    allSamplesLoaded &= loadSample(fullPath + "glass-brick-break.ogg", SND_GLASS_BRICK_BREAK);
    allSamplesLoaded &= loadSample(fullPath + "invisible-brick-appear.ogg", SND_INVISIBLE_BRICK_APPEAR);
    allSamplesLoaded &= loadSample(fullPath + "cement-brick-hit.ogg", SND_CEMENT_BRICK_HIT);
    allSamplesLoaded &= loadSample(fullPath + "doom-brick-break.ogg", SND_DOOM_BRICK_BREAK);
    allSamplesLoaded &= loadSample(fullPath + "po-hit-border.ogg", SND_PO_HIT_BORDER);
    allSamplesLoaded &= loadSample(fullPath + "good-po-hit-paddle.ogg", SND_GOOD_PO_HIT_PADDLE);
    allSamplesLoaded &= loadSample(fullPath + "evil-po-hit-paddle.ogg", SND_EVIL_PO_HIT_PADDLE);
    allSamplesLoaded &= loadSample(fullPath + "shot.ogg", SND_SHOT);
    allSamplesLoaded &= loadSample(fullPath + "die.ogg", SND_DIE);
    allSamplesLoaded &= loadSample(fullPath + "nextlevel.ogg", SND_NEXTLEVEL);
    allSamplesLoaded &= loadSample(fullPath + "gameover.ogg", SND_GAMEOVER);
    allSamplesLoaded &= loadSample(fullPath + "highscore.ogg", SND_HIGHSCORE);
    allSamplesLoaded &= loadSample(fullPath + "menuclick.ogg", SND_MENUCLICK);
    allSamplesLoaded &= loadSample(fullPath + "glue-ball-hit-paddle.ogg", SND_GLUE_BALL_HIT_PADDLE);
    allSamplesLoaded &= loadSample(fullPath + "buy-powerup.ogg", SND_BUY_POWERUP);

    if (!allSamplesLoaded) {
        SDL_Log("Not all samples loaded. themen-change not complete.");
    }

    return allSamplesLoaded;
}

bool SoundManager::loadMusic(const std::string &musicPath) {
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
    currentMusic = Mix_LoadMUS(musicPath.c_str());
    if (!currentMusic) {
        SDL_Log("Mix_LoadMUS failed: %s", Mix_GetError());
        return false;
    }
    return true;
}

void SoundManager::playMusic(int loops) {
    if (currentMusic) {
        Mix_PlayMusic(currentMusic, loops);
    }
}

void SoundManager::stopMusic() {
    Mix_HaltMusic();
}
