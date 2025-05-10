#include <vector>
#include <SDL2/SDL_mixer.h>
#include "SoundManager.h"

#include <SDL_log.h>
#include <string>

#include "SettingsManager.h"
#include "ThemeManager.h"

extern settings setting;
extern ThemeManager themeManager;

bool SoundManager::init() {
    constexpr Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
    constexpr int audio_channels = 2;
    constexpr int audio_buffers = 1024;
    constexpr int audio_rate = 44100;
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
        SDL_Log("Error: Could not open audio device.Sound have been disabled.");
        setting.sound = false;
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

    if (setting.sound)
        loadSounds();
    return true;
}

void SoundManager::loadSample(const char *SampleName, int sampleNum) {
    std::string F = "snd/";
    F += SampleName;

    sample[sampleNum] = Mix_LoadWAV(themeManager.getThemeFilePath(F, setting.sndTheme).data());
    if (!sample[sampleNum]) {
        SDL_Log("SoundManager '%s' :%s", F.c_str(), Mix_GetError());
    }
}

/* This function puts a sample in queue for playing */
void SoundManager::add(int i, GLfloat x) {
    if (setting.sound == 0)
        return;

    int p = 0;
    if (setting.stereo) {
        p = (255.0 / 3.2) * (x + 1.6);
    }

    sampleQueuedItem qt;
    qt.s = i;
    qt.p = p;
    q.push_back(qt);
}

void SoundManager::loadSounds() {
    loadSample("start.ogg", SND_START);
    loadSample("ball-hit-border.ogg", SND_BALL_HIT_BORDER);
    loadSample("ball-hit-paddle.ogg", SND_BALL_HIT_PADDLE);
    loadSample("norm-brick-breaka.ogg", SND_NORM_BRICK_BREAK); //breaka

    loadSample("norm-brick-breakb.ogg", SND_NORM_BRICK_BREAKB);
    loadSample("norm-brick-breakc.ogg", SND_NORM_BRICK_BREAKC);
    loadSample("norm-brick-breakd.ogg", SND_NORM_BRICK_BREAKD);
    loadSample("norm-brick-breake.ogg", SND_NORM_BRICK_BREAKE);


    loadSample("expl-brick-break.ogg", SND_EXPL_BRICK_BREAK);
    loadSample("glass-brick-hit.ogg", SND_GLASS_BRICK_HIT);
    loadSample("glass-brick-break.ogg", SND_GLASS_BRICK_BREAK);
    loadSample("invisible-brick-appear.ogg", SND_INVISIBLE_BRICK_APPEAR);
    loadSample("cement-brick-hit.ogg", SND_CEMENT_BRICK_HIT);
    loadSample("doom-brick-break.ogg", SND_DOOM_BRICK_BREAK);
    loadSample("po-hit-border.ogg", SND_PO_HIT_BORDER);
    loadSample("good-po-hit-paddle.ogg", SND_GOOD_PO_HIT_PADDLE);
    loadSample("evil-po-hit-paddle.ogg", SND_EVIL_PO_HIT_PADDLE);
    loadSample("shot.ogg", SND_SHOT);
    loadSample("die.ogg", SND_DIE);
    loadSample("nextlevel.ogg", SND_NEXTLEVEL);
    loadSample("gameover.ogg", SND_GAMEOVER);
    loadSample("highscore.ogg", SND_HIGHSCORE);
    loadSample("menuclick.ogg", SND_MENUCLICK);
    loadSample("glue-ball-hit-paddle.ogg", SND_GLUE_BALL_HIT_PADDLE);
    loadSample("buy-powerup.ogg", SND_BUY_POWERUP);
}

/* This function is called only when drawing a frame, and plays the queue of samples,
   It will average the x/stereo position of a sample if it is queued more than once */
void SoundManager::play() {
    if (setting.sound == 0)
        return;

    //Playlist (lol, imagination for the win..)
    std::vector<sampleQueuedItem> pl;
    std::vector<sampleQueuedItem>::iterator plIt;
    bool same = false;
    int freeChannel = -1; //The channel we will use for this sample

    //Loop through queue and find samples thare are the same, average their position and put in a new vector
    for (auto & it : q) {
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
        if (setting.stereo) {
            plIt->p /= plIt->num;
            Mix_SetPanning(freeChannel, 255 - plIt->p, plIt->p);
        }

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
    for (const auto & i : sample) {
        Mix_FreeChunk(i);
    }
    Mix_CloseAudio();
}
