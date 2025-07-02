// SettingsManager_aTests.cpp
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "SettingsManager.h"
#include "config.h"

class SettingsManagerTest : public ::testing::Test {
protected:
    const std::string testDir = "./test_settings";
    SettingsManager *settingsManager;

    void SetUp() override {
        std::filesystem::create_directory(testDir);
        settingsManager = new SettingsManager("./test_settings/settings.txt");
    }

    void TearDown() override {
        delete settingsManager;
        std::filesystem::remove_all(testDir);
    }
};

TEST_F(SettingsManagerTest, DefaultsAreSet) {
    const auto &s = settingsManager->getSettings();
    EXPECT_EQ(s.gfxTheme, DEFAULT_GFX_THEME);
    EXPECT_EQ(s.sndTheme, DEFAULT_SND_THEME);
    EXPECT_EQ(s.lvlTheme, DEFAULT_LVL_THEME);
}
