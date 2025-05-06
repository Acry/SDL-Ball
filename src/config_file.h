#pragma once

#include <string>

/**
 * @brief Handles configuration file paths and directory management
 *
 * This class manages file paths for configuration, save games, high scores
 * and screenshots. It supports both custom and automatically determined root paths
 * following the XDG Base Directory Specification.
 */
class ConfigFile {
    std::string programRoot;
    std::string saveGameFile;
    std::string settingsFile;
    std::string highScoreFile;
    std::string screenshotDir;

public:
    /**
     * @brief Constructs a ConfigFile object
     * @param root Optional custom root path. If empty, the path will be auto-detected
     */
    explicit ConfigFile(std::string root = "");

    /**
     * @brief Initializes paths and creates required directories
     * @return true if initialization successful, false otherwise
     */
    bool init();

    /**
     * @brief Checks if directory exists and creates it if necessary
     * @param dir Directory path to check/create
     * @return true if directory exists or was created, false on error
     */
    static bool checkDir(const std::string &dir);

    // Getter
    [[nodiscard]] const std::string &getProgramRoot() const { return programRoot; }
    [[nodiscard]] const std::string &getSaveGameFile() const { return saveGameFile; }
    [[nodiscard]] const std::string &getSettingsFile() const { return settingsFile; }
    [[nodiscard]] const std::string &getHighScoreFile() const { return highScoreFile; }
    [[nodiscard]] const std::string &getScreenshotDir() const { return screenshotDir; }
};
