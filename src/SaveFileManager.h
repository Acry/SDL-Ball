// SaveFileManager.hpp
#pragma once
#include <string>
#include "ConfigFileManager.h"
#include "game_state.h"

struct savedGame {
    char name[32];
    player_struct player;
};

class SaveFileManager {
public:
    explicit SaveFileManager(ConfigFileManager &cfg);

    void saveGame(int slot, const std::string &name, const player_struct &player) const;
    bool loadGame(int slot, player_struct &player) const;
    void clearSaveGames() const;
    int listSaveGames(std::string slotNames[6]) const;

private:
    ConfigFileManager &configFileManager;
};