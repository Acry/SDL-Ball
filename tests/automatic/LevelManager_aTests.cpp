#include <filesystem>

#include "gtest/gtest.h"
#include "../../src/LevelManager.h"
#include <fstream>


// TODO: Refactoring auf Dependency Injection mit Interfaces.

// Hilfsfunktion zum Erstellen eines Verzeichnisses
void createDirectory(const std::string &dirName) {
    try {
        std::filesystem::create_directories(dirName);
    } catch (const std::exception &e) {
        throw std::runtime_error("Verzeichnis konnte nicht erstellt werden: " + dirName + " - " + e.what());
    }
}

// Hilfsfunktion zum Erstellen einer levels.txt-Datei in einem Theme-Verzeichnis
void createLevelsFile(const std::string &themeDir, const std::vector<std::string> &levelData) {
    createDirectory(themeDir);
    std::string filename = themeDir + "/levels.txt";

    if (std::ofstream file(filename); file.is_open()) {
        file << "** Start **\n";
        for (const auto &row: levelData) {
            file << row << "\n";
        }
        file << "** Stop **\n";
        file.close();
    } else {
        throw std::runtime_error("Level-Datei konnte nicht erstellt werden: " + filename);
    }
}

// Hilfsfunktion zum Aufräumen nach den Tests
void cleanupDirectory(const std::string &dirName) {
    try {
        std::filesystem::remove_all(dirName);
    } catch (const std::exception &) {
        // Fehler beim Löschen ignorieren
    }
}

TEST(LevelTest, InvalidDir) {
    LevelManager levelManager;
    const std::string dirName = "no_level_dir";
    const bool result = levelManager.setTheme(dirName);
    ASSERT_FALSE(result);
    cleanupDirectory(dirName);
}

TEST(LevelTest, EmptyLevel) {
    LevelManager levelManager;
    const std::string dirName = "empty_level_dir";
    createLevelsFile(dirName, {}); // Leere Level-Daten
    const bool result = levelManager.setTheme(dirName);
    ASSERT_FALSE(result);
    cleanupDirectory(dirName);
}

TEST(LevelTest, EmptySimpleLevel) {
    const std::string dirName = "simple_level_dir";
    createLevelsFile(dirName, {
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000"
                     });

    LevelManager levelManager;
    const bool result = levelManager.setTheme(dirName);
    ASSERT_TRUE(result);
    const auto expectedName = dirName + "/levels.txt";
    ASSERT_EQ(levelManager.getCurrentTheme(), expectedName);
    cleanupDirectory(dirName);
}

TEST(LevelTest, LoadLevelWithOneBrick) {
    const std::string dirName = "one_brick_level_dir";
    createLevelsFile(dirName, {
                         "0800000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000",
                         "0000000000000000000000000000000000000000000000000000"
                     });

    LevelManager levelManager;
    const bool result = levelManager.setTheme(dirName);
    ASSERT_TRUE(result);
    cleanupDirectory(dirName);
}
