// SaveFileManager.cpp
#include "SaveFileManager.h"
#include <fstream>
#include <cstring>
#include <SDL2/SDL_log.h>

#include "config.h"

SaveFileManager::SaveFileManager(ConfigFile &cfg) : configFile(cfg) {}

void SaveFileManager::saveGame(const int slot, const std::string &name, const player_struct &player) const {
    std::fstream file;
    savedGame game;
    std::strncpy(game.name, name.c_str(), sizeof(game.name));
    game.name[sizeof(game.name) - 1] = '\0';
    game.player = player;
    file.open(configFile.getSaveGameFile().c_str(), std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) return;
    file.seekp(sizeof(int) + (sizeof(savedGame) * slot));
    file.write(reinterpret_cast<char*>(&game), sizeof(savedGame));
    file.close();
}

bool SaveFileManager::loadGame(const int slot, player_struct &player) const {
    std::fstream file;
    savedGame game;
    file.open(configFile.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("Could not open '%s' for Reading.", configFile.getSaveGameFile().c_str());
        return false;
    }
    file.seekg(sizeof(int) + (sizeof(savedGame) * slot));
    file.read(reinterpret_cast<char*>(&game), sizeof(savedGame));
    file.close();
    if (game.player.level != 0) {
        player = game.player;
        return true;
    }
    return false;
}

void SaveFileManager::clearSaveGames() const {
    std::fstream file;
    file.open(configFile.getSaveGameFile().c_str(), std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("Could not open '%s' for Read+Write.", configFile.getSaveGameFile().c_str());
        return;
    }
    //Write the header
    constexpr int sgHead = SAVEGAME_VERSION;
    file.write(reinterpret_cast<const char*>(&sgHead), sizeof(int));
    file.close();
    for (int i = 0; i < 6; ++i)
        saveGame(i, "Empty Slot", player_struct{});
}

int SaveFileManager::listSaveGames(std::string slotNames[6]) const {
    std::fstream file;
    savedGame slot[6];
    int i = 0;
    file.open(configFile.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) return 0;
    int sgHead = 0;
    file.read(reinterpret_cast<char*>(&sgHead), sizeof(int));
    if (sgHead != SAVEGAME_VERSION) {
        file.close();
        clearSaveGames();
        file.open(configFile.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
        file.seekp(sizeof(int));
    }
    while (i < 6) {
        file.read(reinterpret_cast<char*>(&slot[i]), sizeof(savedGame));
        if (file.eof()) break;
        slotNames[i] = slot[i].name;
        i++;
    }
    file.close();
    return i;
}
