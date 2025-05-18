#include "SaveGameManager.h"
#include <fstream>
#include <cstring>
#include <SDL2/SDL_log.h>

#include "config.h"

SaveGameManager::SaveGameManager(ConfigFileManager &cfg) : configFileManager(cfg) {}

void SaveGameManager::saveGame(const int slot, const std::string &name, const player_struct &player) const {
    std::fstream file;
    savedGame game;
    std::strncpy(game.name, name.c_str(), sizeof(game.name));
    game.name[sizeof(game.name) - 1] = '\0';
    game.player = player;
    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) return;
    file.seekp(sizeof(int) + (sizeof(savedGame) * slot));
    file.write(reinterpret_cast<char*>(&game), sizeof(savedGame));
    file.close();
}

bool SaveGameManager::loadGame(const int slot, player_struct &player) const {
    std::fstream file;
    savedGame game;
    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("Could not open '%s' for Reading.", configFileManager.getSaveGameFile().c_str());
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

void SaveGameManager::clearSaveGames() const {
    std::fstream file;
    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::out | std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("Could not open '%s' for Read+Write.", configFileManager.getSaveGameFile().c_str());
        return;
    }
    //Write the header
    constexpr int sgHead = SAVEGAME_VERSION;
    file.write(reinterpret_cast<const char*>(&sgHead), sizeof(int));
    file.close();
    for (int i = 0; i < 6; ++i)
        saveGame(i, "Empty Slot", player_struct{});
}

int SaveGameManager::listSaveGames(std::string slotNames[6]) const {
    std::fstream file;
    bool isLegacyFormat = false;
    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);

    // Datei existiert nicht oder ist leer
    if (!file.is_open()) return 0;

    // Versuche, den Header zu lesen
    int sgHead = 0;
    file.read(reinterpret_cast<char*>(&sgHead), sizeof(int));

    // Prüfe, ob es sich um einen gültigen Header handelt
    if (sgHead != SAVEGAME_VERSION) {
        // Wenn nicht, könnte es sich um v1-Format handeln
        file.close();

        // Prüfe, ob es sich um ein altes Format handelt
        isLegacyFormat = detectLegacyFormat();

        if (isLegacyFormat) {
            SDL_Log("Legacy-Savegame (Version 1) erkannt. Konvertiere...");
            convertLegacyToCurrentFormat();
        } else {
            // Kein erkanntes Format, erstelle neu
            clearSaveGames();
        }

        file.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
        if (!file.is_open()) return 0;

        // Skip header
        file.seekg(sizeof(int));
    }

    // Rest wie bisher
    savedGame slot[6];
    int i = 0;
    while (i < 6) {
        file.read(reinterpret_cast<char*>(&slot[i]), sizeof(savedGame));
        if (file.eof()) break;
        slotNames[i] = slot[i].name;
        i++;
    }
    file.close();
    return i;
}

// Füge diese Methode zur SaveGameManager-Klasse hinzu
bool SaveGameManager::loadLegacyGame(int slot, player_struct &player) const {
    std::fstream file;
    savedGame game;

    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("Konnte Legacy-Savegame '%s' nicht öffnen", configFileManager.getSaveGameFile().c_str());
        return false;
    }

    // Direkt zu Version 1 Position springen (ohne Header)
    file.seekg(sizeof(savedGame) * slot);
    file.read(reinterpret_cast<char*>(&game), sizeof(savedGame));

    if (file.fail() || file.eof()) {
        file.close();
        return false;
    }

    file.close();

    // Überprüfe, ob es sich um einen gültigen Spielstand handelt
    if (game.player.level != 0) {
        // Kopiere den Spielstand
        player = game.player;
        return true;
    }

    return false;
}

bool SaveGameManager::detectLegacyFormat() const {
    std::fstream file;
    file.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    // Versuche, Version 1 Spielstände zu erkennen
    // Wir prüfen, ob mindestens ein gültiger Slot vorhanden ist
    for (int slot = 0; slot < 6; slot++) {
        savedGame game;
        file.seekg(sizeof(savedGame) * slot);
        file.read(reinterpret_cast<char*>(&game), sizeof(savedGame));

        if (file.fail() || file.eof()) {
            file.close();
            return false;
        }

        // Überprüfe, ob der Name des Slots sinnvoll erscheint
        bool validName = true;
        for (int i = 0; i < sizeof(game.name) && game.name[i] != '\0'; i++) {
            if (!isprint(static_cast<unsigned char>(game.name[i]))) {
                validName = false;
                break;
            }
        }

        // Prüfe, ob Level im sinnvollen Bereich ist
        if (validName && game.player.level >= 0 && game.player.level <= 100) {
            file.close();
            return true;  // Wahrscheinlich ein Legacy-Format
        }
    }

    file.close();
    return false;
}
void SaveGameManager::convertLegacyToCurrentFormat() const {
    std::fstream fileIn, fileOut;
    savedGame slots[6];

    // Öffne die alte Datei
    fileIn.open(configFileManager.getSaveGameFile().c_str(), std::ios::in | std::ios::binary);
    if (!fileIn.is_open()) return;

    // Lese alle Slots ein
    for (int i = 0; i < 6; i++) {
        fileIn.seekg(sizeof(savedGame) * i);
        fileIn.read(reinterpret_cast<char*>(&slots[i]), sizeof(savedGame));
        if (fileIn.fail()) {
            // Bei Fehler: Initialisiere leeren Slot
            std::strncpy(slots[i].name, "Empty Slot", sizeof(slots[i].name));
            slots[i].player = player_struct{};
        }
    }
    fileIn.close();

    // Erstelle eine temporäre Datei für die konvertierte Version
    std::string tempFile = configFileManager.getSaveGameFile() + ".tmp";
    fileOut.open(tempFile.c_str(), std::ios::out | std::ios::binary);
    if (!fileOut.is_open()) return;

    // Schreibe den Header
    constexpr int sgHead = SAVEGAME_VERSION;
    fileOut.write(reinterpret_cast<const char*>(&sgHead), sizeof(int));

    // Schreibe die konvertierten Slots
    for (int i = 0; i < 6; i++) {
        fileOut.write(reinterpret_cast<char*>(&slots[i]), sizeof(savedGame));
    }

    fileOut.close();

    // Ersetze die alte Datei durch die neue
    if (std::rename(tempFile.c_str(), configFileManager.getSaveGameFile().c_str()) != 0) {
        SDL_Log("Fehler beim Umbenennen der konvertierten Datei");
    }

    SDL_Log("Altes Format zu Version %d konvertiert", SAVEGAME_VERSION);
}
