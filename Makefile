PREFIX?=~/.local

DATADIR?=$(PREFIX)share/sdl-ball/themes/
BINDIR?=$(PREFIX)/bin/

CXX = clang++

COMMON_FLAGS := -std=c++23 -Wall -Wextra -mtune=native $(shell sdl2-config --cflags)
LDFLAGS := -lepoxy -lGLU  $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
DEBUG_FLAGS := -ggdb -gdwarf-5 -O0 -Wall -DDEBUG -fdebug-types-section -DDATADIR="\"./../\"" $(COMMON_FLAGS)
RELEASE_FLAGS := -O3 -DNDEBUG $(COMMON_FLAGS)

# directories
BUILD_DIR := build/
SOURCE_DIR := src/

SOURCES := $(addprefix $(SOURCE_DIR), \
    MathHelper.cpp \
    Display.cpp \
    main.cpp \
    ConfigFileManager.cpp \
    SettingsManager.cpp \
    TtfLegacyGl.cpp \
    SaveGameManager.cpp \
    SoundManager.cpp \
    ThemeManager.cpp \
    SpriteSheetAnimation.cpp \
    game_state.cpp \
    Score.cpp \
    BackgroundManager.cpp \
    TextureManager.cpp \
    Speedometer.cpp \
    Paddle.cpp \
    Tracer.cpp \
    Ball.cpp \
)

# Create the build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOURCES:.cpp=.o)))
TEST_TARGETS := config-test settings-test display-test spritesheet-test paddle-test
TARGET=sdl-ball
GAME_OBJECTS := $(OBJECTS)

.PHONY: all clean-all debug release install install-bin install-data game tests

all: game tests

game: release debug

tests: $(TEST_TARGETS)

release: $(BUILD_DIR)$(TARGET)

debug: $(GAME_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(GAME_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)$(TARGET)

$(BUILD_DIR)$(TARGET): $(OBJECTS)
	$(CXX) $(RELEASE_FLAGS) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)%.o: $(SOURCE_DIR)%.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

.PHONY: clean-all
clean-all:
	@rm -rf $(BUILD_DIR) 2>/dev/null || true

install: $(BUILD_DIR)$(TARGET) install-bin install-data

install-bin:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BUILD_DIR)$(TARGET) $(DESTDIR)$(BINDIR)

install-data:
	mkdir -p $(DESTDIR)$(DATADIR)
	cp -p -R themes/* $(DESTDIR)$(DATADIR)

remove-config:
	rm -R ~/.config/sdl-ball

###############################################################################
# TEST TARGETS
###############################################################################

###############################################################################
# ConfigManager
CONFIG_TEST_SOURCES := $(SOURCE_DIR)ConfigFileManager_Tests.cpp \
                       $(SOURCE_DIR)ConfigFileManager.cpp

CONFIG_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONFIG_TEST_SOURCES:.cpp=.o)))

config-test: $(CONFIG_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONFIG_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)config-test

$(BUILD_DIR)ConfigFileManager_Tests.o: $(SOURCE_DIR)ConfigFileManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)ConfigFileManager.o: $(SOURCE_DIR)ConfigFileManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# SettingsManager
SETTINGS_TEST_SOURCES := $(SOURCE_DIR)SettingsManager_Tests.cpp \
                         $(SOURCE_DIR)ConfigFileManager.cpp \
                         $(SOURCE_DIR)SettingsManager.cpp

SETTINGS_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SETTINGS_TEST_SOURCES:.cpp=.o)))

settings-test: $(SETTINGS_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SETTINGS_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)settings-test

$(BUILD_DIR)settings_manager.o: $(SOURCE_DIR)SettingsManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# DisplayManager
DISPLAY_TEST_SOURCES := $(SOURCE_DIR)BackgroundManager_Tests.cpp \
                        $(SOURCE_DIR)Display.cpp \
                        $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                        $(SOURCE_DIR)TextureManager.cpp \
                        $(SOURCE_DIR)BackgroundManager.cpp

display-test: $(DISPLAY_TEST_SOURCES)
	$(CXX) $(DEBUG_FLAGS) $(DISPLAY_TEST_SOURCES) $(shell sdl2-config --libs) -lepoxy -lSDL2_image -o $(BUILD_DIR)display-test

$(BUILD_DIR)Display.o: $(SOURCE_DIR)Display.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)Texture.o: $(SOURCE_DIR)SpriteSheetAnimation.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)TextureManager.o: $(SOURCE_DIR)TextureManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)BackgroundManager.o: $(SOURCE_DIR)BackgroundManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# SpriteSheetAnimation
SPRITESHEET_TEST_SOURCES := $(SOURCE_DIR)SpriteSheetAnimation_Tests.cpp \
                            $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                            $(SOURCE_DIR)Display.cpp \
                            $(SOURCE_DIR)TextureManager.cpp \

SPRITESHEET_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SPRITESHEET_TEST_SOURCES:.cpp=.o)))

spritesheet-test: $(SPRITESHEET_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SPRITESHEET_TEST_OBJECTS) $(shell sdl2-config --libs) -lepoxy -lSDL2_image -o $(BUILD_DIR)spritesheet-test

###############################################################################
# Paddle
PADDLE_TEST_SOURCES := $(SOURCE_DIR)Paddle_Tests.cpp \
                       $(SOURCE_DIR)Paddle.cpp \
                       $(SOURCE_DIR)GameObject.cpp \
                       $(SOURCE_DIR)Display.cpp \
                       $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                       $(SOURCE_DIR)TextureManager.cpp

PADDLE_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(PADDLE_TEST_SOURCES:.cpp=.o)))

paddle-test: $(PADDLE_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(PADDLE_TEST_OBJECTS) $(shell sdl2-config --libs) -lepoxy -lSDL2_image -o $(BUILD_DIR)paddle-test

###############################################################################
# ThemeManager
THEME_TEST_SOURCES := $(SOURCE_DIR)ThemeManager_Tests.cpp \
                      $(SOURCE_DIR)ThemeManager.cpp \
                      $(SOURCE_DIR)ConfigFileManager.cpp

THEME_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(THEME_TEST_SOURCES:.cpp=.o)))

theme-test: $(THEME_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(THEME_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)theme-test

###############################################################################
# TextManager
TEXT_TEST_SOURCES := $(SOURCE_DIR)TextManager_Tests.cpp \
                     $(SOURCE_DIR)TtfLegacyGl.cpp \
                     $(SOURCE_DIR)Display.cpp \
                     $(SOURCE_DIR)TextureManager.cpp

TEXT_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TEXT_TEST_SOURCES:.cpp=.o)))

text-test: $(TEXT_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(TEXT_TEST_OBJECTS) $(shell sdl2-config --libs) -lepoxy -lGLU -lSDL2_ttf -lSDL2_image -o $(BUILD_DIR)text-test
