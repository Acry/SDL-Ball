// ConfigFileManager_aTests.cpp
#include <gtest/gtest.h>
#include "ConfigFileManager.h"
#include <filesystem>

class ConfigFileManagerTest : public ::testing::Test {
protected:
    const std::string testDir = "./test_config";
    ConfigFileManager *configManager;

    void SetUp() override {
        std::filesystem::create_directory(testDir);
        configManager = new ConfigFileManager(testDir);
    }

    void TearDown() override {
        delete configManager;
        std::filesystem::remove_all(testDir);
    }
};

TEST_F(ConfigFileManagerTest, InitCreatesDirectories) {
    EXPECT_TRUE(std::filesystem::exists(testDir));
    EXPECT_TRUE(std::filesystem::exists(configManager->getProgramRoot()));
    EXPECT_TRUE(std::filesystem::exists(configManager->getScreenshotDir()));
}

TEST_F(ConfigFileManagerTest, GettersReturnValidPaths) {
    EXPECT_FALSE(configManager->getProgramRoot().empty());
    EXPECT_FALSE(configManager->getSettingsFile().empty());
    EXPECT_FALSE(configManager->getSaveGameFile().empty());
    EXPECT_FALSE(configManager->getHighScoreFile().empty());
    EXPECT_FALSE(configManager->getScreenshotDir().empty());
}

TEST_F(ConfigFileManagerTest, ThemeDirectoriesExist) {
    EXPECT_FALSE(configManager->getUserThemeDir().empty());
    EXPECT_FALSE(ConfigFileManager::getGlobalThemeDir().empty());
}

TEST_F(ConfigFileManagerTest, CheckDirCreatesDirectory) {
    const std::string newDir = testDir + "/newdir";
    EXPECT_TRUE(ConfigFileManager::checkDir(newDir));
    EXPECT_TRUE(std::filesystem::exists(newDir));
}
