#include <vector>
#include <filesystem>
#include <SDL_log.h>
#include <string>
#include <SDL2/SDL_mixer.h>

#include "SoundManager.h"
#include "EventManager.h"

SoundManager::SoundManager() : sample{}, q{}, soundThemePath{}, currentChannels{0}, breakSoundIndex{0} {
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

bool SoundManager::loadSample(const char *SampleName, const int sampleNum) {
    const std::string fullFontPath = soundThemePath + "/" + SampleName;
    sample[sampleNum] = Mix_LoadWAV(fullFontPath.c_str());
    if (!sample[sampleNum]) {
        SDL_Log("SoundManager '%s' :%s", fullFontPath.c_str(), Mix_GetError());
        return false;
    }
    return true;
}

/* This function puts a sample in the queue for playing */
void SoundManager::queueSound(const int i, const GLfloat x) {
    const int p = (255.0 / 3.2) * (x + 1.0);
    sampleQueuedItem qt;
    qt.s = i;
    qt.p = p;
    q.push_back(qt);
}

bool SoundManager::setSoundTheme(const std::string &soundTheme) {
    clearSoundTheme();
    const std::filesystem::path path(soundTheme);
    soundThemePath = path.parent_path().string();

    // 25 Sounds -> USED_SOUND_SAMPLES
    bool allSamplesLoaded = true;

    // Samples nacheinander laden und Fehler prüfen
    allSamplesLoaded &= loadSample("start.ogg", SND_START);
    allSamplesLoaded &= loadSample("ball-hit-border.ogg", SND_BALL_HIT_BORDER);
    allSamplesLoaded &= loadSample("ball-hit-paddle.ogg", SND_BALL_HIT_PADDLE);
    allSamplesLoaded &= loadSample("norm-brick-breaka.ogg", SND_NORM_BRICK_BREAK);
    allSamplesLoaded &= loadSample("norm-brick-breakb.ogg", SND_NORM_BRICK_BREAKB);
    allSamplesLoaded &= loadSample("norm-brick-breakc.ogg", SND_NORM_BRICK_BREAKC);
    allSamplesLoaded &= loadSample("norm-brick-breakd.ogg", SND_NORM_BRICK_BREAKD);
    allSamplesLoaded &= loadSample("norm-brick-breake.ogg", SND_NORM_BRICK_BREAKE);
    allSamplesLoaded &= loadSample("expl-brick-break.ogg", SND_EXPL_BRICK_BREAK);
    allSamplesLoaded &= loadSample("glass-brick-hit.ogg", SND_GLASS_BRICK_HIT);
    allSamplesLoaded &= loadSample("glass-brick-break.ogg", SND_GLASS_BRICK_BREAK);
    allSamplesLoaded &= loadSample("invisible-brick-appear.ogg", SND_INVISIBLE_BRICK_APPEAR);
    allSamplesLoaded &= loadSample("cement-brick-hit.ogg", SND_CEMENT_BRICK_HIT);
    allSamplesLoaded &= loadSample("doom-brick-break.ogg", SND_DOOM_BRICK_BREAK);
    allSamplesLoaded &= loadSample("po-hit-border.ogg", SND_PO_HIT_BORDER);
    allSamplesLoaded &= loadSample("good-po-hit-paddle.ogg", SND_GOOD_PO_HIT_PADDLE);
    allSamplesLoaded &= loadSample("evil-po-hit-paddle.ogg", SND_EVIL_PO_HIT_PADDLE);
    allSamplesLoaded &= loadSample("shot.ogg", SND_SHOT);
    allSamplesLoaded &= loadSample("die.ogg", SND_DIE);
    allSamplesLoaded &= loadSample("nextlevel.ogg", SND_NEXTLEVEL);
    allSamplesLoaded &= loadSample("gameover.ogg", SND_GAMEOVER);
    allSamplesLoaded &= loadSample("highscore.ogg", SND_HIGHSCORE);
    allSamplesLoaded &= loadSample("menuclick.ogg", SND_MENUCLICK);
    allSamplesLoaded &= loadSample("glue-ball-hit-paddle.ogg", SND_GLUE_BALL_HIT_PADDLE);
    allSamplesLoaded &= loadSample("buy-powerup.ogg", SND_BUY_POWERUP);

    if (!allSamplesLoaded) {
        SDL_Log("Nicht alle Samples konnten geladen werden. Theme-Wechsel unvollständig.");
    }

    return allSamplesLoaded;
}

/* This function is called only when drawing a frame, and plays the queue of samples,
   It will average the x/stereo position of a sample if it is queued more than once */
void SoundManager::play() {
    //Playlist (lol, imagination for the win...)
    std::vector<sampleQueuedItem> pl;
    std::vector<sampleQueuedItem>::iterator plIt;
    bool same = false;
    int freeChannel = -1; //The channel we will use for this sample

    //Loop through queue and find samples thare are the same, average their position and put in a new vector
    for (auto &it: q) {
        //Loop thrugh the playlist to see find out if this allready exist
        same = false;
        for (plIt = pl.begin(); plIt != pl.end(); ++plIt) {
            if (plIt->s == it.s) {
                same = true;
                plIt->num++;
                plIt->p += it.p;
            }
        }

        //this sample is not yet in the playlist
        if (!same) {
            pl.push_back(it);
            plIt = pl.end();
            --plIt;
            plIt->num = 1;
        }
    }
    q.clear();

    //Play the actual samples :)
    for (plIt = pl.begin(); plIt != pl.end(); ++plIt) {
        //Find a free channel:
        for (int i = 0; i < currentChannels; i++) {
            if (!Mix_Playing(i)) {
                freeChannel = i;
                break;
            }
        }
        plIt->p /= plIt->num;
        Mix_SetPanning(freeChannel, 255 - plIt->p, plIt->p);

        if (plIt->s == SND_NORM_BRICK_BREAK) {
            switch (breakSoundIndex) {
                //Case 0 = SND_NORM_BREAK(a)
                case 1:
                    plIt->s = SND_NORM_BRICK_BREAKB;
                    break;
                case 2:
                    plIt->s = SND_NORM_BRICK_BREAKC;
                    break;
                case 3:
                    plIt->s = SND_NORM_BRICK_BREAKD;
                    break;
                case 4:
                    plIt->s = SND_NORM_BRICK_BREAKE;
                    break;
                default: ;
            }

            breakSoundIndex++;
            if (breakSoundIndex == 5)
                breakSoundIndex = 0;
        }

        if (Mix_PlayChannel(freeChannel, sample[plIt->s], 0) == -1) {
            printf("Sample %i: %s\n", plIt->s, Mix_GetError());
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
    clearSoundTheme();
    Mix_CloseAudio();
    Mix_Quit();
}

void SoundManager::clearSoundTheme() {
    for (const auto &i: sample) {
        Mix_FreeChunk(i);
    }
}

void SoundManager::registerEvents(EventManager* evManager) {
    if (!evManager) return;

    eventManager = evManager;

    // Kollisions-Events mit Sounds verknüpfen
    eventManager->addListener(GameEvent::BallHitLeftBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);

    eventManager->addListener(GameEvent::BallHitRightBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);

    eventManager->addListener(GameEvent::BallHitTopBorder, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_BORDER, data.posX);
    }, this);

    eventManager->addListener(GameEvent::BallHitPaddle, [this](const EventData& data) {
        this->queueSound(SND_BALL_HIT_PADDLE, data.posX);
    }, this);

    eventManager->addListener(GameEvent::BrickDestroyed, [this](const EventData& data) {
        this->queueSound(SND_NORM_BRICK_BREAK, data.posX);
    }, this);

    // FIXME
    eventManager->addListener(GameEvent::BallHitBrick, [this](const EventData& data) {
        this->queueSound(SND_NORM_BRICK_BREAK, data.posX);
    }, this);
    // Weitere Event-Listener hinzufügen...
}