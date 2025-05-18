// SaveFileManager.hpp
#pragma once
#include <string>
#include "ConfigFileManager.h"
#include "game_state.h"

// have savegame version 1
// playerstruct version 1
// have savegame version 2
// playerstruct version 2
// current player
#define SAVEGAME_VERSION_2 2
#define SAVEGAME_VERSION_1 1
#define SAVEGAME_CURRENT_VERSION 3.1

// MAXPOTEXTURES needs to be decoupled
struct player_struct_version_1 {
    int multiply;
    bool powerup[MAXPOTEXTURES];
    bool explodePaddle; //This lock makes the paddle explode and it won't come back until newlife.
    int level;
    int lives;
    int difficulty;
    int score;
};

struct player_struct_version_2 {
    int coins;
    int multiply;
    bool powerup[MAXPOTEXTURES];
    bool explodePaddle; //This lock makes the paddle explode and it won't come back until newlife.
    int level;
    int lives;
    int difficulty;
    int score;
};

struct savedGame {
    char name[32];
    player_struct player;
};

class SaveGameManager {
public:
    explicit SaveGameManager(ConfigFileManager &cfg);
    void saveGame(int slot, const std::string &name, const player_struct &player) const;
    bool loadGame(int slot, player_struct &player) const;
    void clearSaveGames() const;
    int listSaveGames(std::string slotNames[6]) const;

private:
    bool loadLegacyGame(int slot, player_struct &player) const;
    void SaveGameManager::convertLegacyToCurrentFormat() const;
    bool SaveGameManager::detectLegacyFormat() const;
    ConfigFileManager &configFileManager;
};
