#pragma once

#include <string>

class ConfigFile {

  std::string programRoot;
  std::string saveGameFile;
  std::string settingsFile;
  std::string highScoreFile;
  std::string screenshotDir;

public:
  explicit ConfigFile(const std::string & root = "");
  bool init();
  bool checkDir(const std::string& dir);

  // Getter
  const std::string& getProgramRoot() const { return programRoot; }
  const std::string& getSaveGameFile() const { return saveGameFile; }
  const std::string& getSettingsFile() const { return settingsFile; }
  const std::string& getHighScoreFile() const { return highScoreFile; }
  const std::string& getScreenshotDir() const { return screenshotDir; }
};