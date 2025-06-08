#pragma once

#include <string>
#include <vector>

/**
 * @brief Handles configuration file paths and directory management
 *
 * This class manages file paths for configuration, save games, high scores
 * and screenshots. It supports both custom and automatically determined root paths
 * following the XDG Base Directory Specification.
 */
class ConfigFileManager {
    std::string programRoot;
    std::string saveGameFile;
    std::string settingsFile;
    std::string highScoreFile;
    std::string screenshotDir;
    std::vector<std::string> themeRoots;

public:
    /**
     * @brief Constructs a ConfigFileManager object
     * @param root Optional custom root path. If empty, the path will be auto-detected
     */
    explicit ConfigFileManager(std::string root = "");

    [[nodiscard]] std::string getUserThemeDir() const;

    [[nodiscard]] static std::string getGlobalThemeDir();

    /**
     * @brief Initializes paths and creates required directories
     * @return true if initialization successful, false otherwise
     */
    bool init();

    [[nodiscard]] const std::vector<std::string> &getThemeRoots() const { return themeRoots; }

    void initThemeRoots();

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
