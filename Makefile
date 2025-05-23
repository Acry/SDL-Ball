gluPREFIX?=~/.local

DATADIR?=$(PREFIX)share/sdl-ball/themes/
BINDIR?=$(PREFIX)/bin/

CXX = clang++

COMMON_FLAGS := -std=c++23 -Wall -Wextra -mtune=native $(shell sdl2-config --cflags)
LDFLAGS := -lepoxy $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
DEBUG_FLAGS := -ggdb -gdwarf-5 -O0 -Wall -DDEBUG -fdebug-types-section -DDATADIR="\"./../\"" $(COMMON_FLAGS)
RELEASE_FLAGS := -O3 -DNDEBUG $(COMMON_FLAGS)

# directories
BUILD_DIR := build/
SOURCE_DIR := src/
MANUAL_TEST_DIR := tests/manual/

SOURCES := $(addprefix $(SOURCE_DIR), \
    BackgroundManager.cpp \
    Ball.cpp \
    CollisionManager.cpp \
    ConfigFileManager.cpp \
    Display.cpp \
    EffectManager.cpp \
    EventManager.cpp \
    GrowableObject.cpp \
    MathHelper.cpp \
    Paddle.cpp \
    PlayfieldBorder.cpp \
    SaveGameManager.cpp \
    Score.cpp \
    SettingsManager.cpp \
    SoundManager.cpp \
    Speedometer.cpp \
    SpriteSheetAnimation.cpp \
    TextManager.cpp \
    TextureManager.cpp \
    ThemeManager.cpp \
    Tracer.cpp \
    game_state.cpp \
    main.cpp \
)

# Create the build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOURCES:.cpp=.o)))
TEST_TARGETS := config-test settings-test display-test spritesheet-test paddle-test theme-test text-test sound-test ball-test effect-test
TARGET=sdl-ball
GAME_OBJECTS := $(OBJECTS)

.PHONY: all clean-all clean debug release install install-bin install-data game tests clean-tests

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
# This section contains the test targets for various components of the project.
###############################################################################
# ConfigManager
CONFIG_TEST_SOURCES := $(MANUAL_TEST_DIR)ConfigFileManager_Tests.cpp \
                       $(SOURCE_DIR)ConfigFileManager.cpp \

CONFIG_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONFIG_TEST_SOURCES:.cpp=.o)))

config-test: $(CONFIG_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONFIG_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)config-test

$(BUILD_DIR)ConfigFileManager_Tests.o: $(MANUAL_TEST_DIR)ConfigFileManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

$(BUILD_DIR)ConfigFileManager.o: $(SOURCE_DIR)ConfigFileManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# SettingsManager
SETTINGS_TEST_SOURCES := $(MANUAL_TEST_DIR)SettingsManager_Tests.cpp \
                         $(SOURCE_DIR)SettingsManager.cpp \
                         $(SOURCE_DIR)ConfigFileManager.cpp \

SETTINGS_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SETTINGS_TEST_SOURCES:.cpp=.o)))

settings-test: $(SETTINGS_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SETTINGS_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)settings-test

$(BUILD_DIR)SettingsManager_Tests.o: $(MANUAL_TEST_DIR)SettingsManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

$(BUILD_DIR)settings_manager.o: $(SOURCE_DIR)SettingsManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# DisplayManager, also used for TextureManager and BackgroundManager
DISPLAY_TEST_SOURCES := $(MANUAL_TEST_DIR)BackgroundManager_Tests.cpp \
                        $(SOURCE_DIR)Display.cpp \
                        $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                        $(SOURCE_DIR)TextureManager.cpp \
                        $(SOURCE_DIR)BackgroundManager.cpp

display-test: $(DISPLAY_TEST_SOURCES)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(DISPLAY_TEST_SOURCES) $(LDFLAGS) -o $(BUILD_DIR)display-test

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
SPRITESHEET_TEST_SOURCES := $(MANUAL_TEST_DIR)SpriteSheetAnimation_Tests.cpp \
                            $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                            $(SOURCE_DIR)Display.cpp \
                            $(SOURCE_DIR)TextureManager.cpp \

SPRITESHEET_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SPRITESHEET_TEST_SOURCES:.cpp=.o)))

spritesheet-test: $(SPRITESHEET_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(SPRITESHEET_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)spritesheet-test

$(BUILD_DIR)SpriteSheetAnimation_Tests.o: $(MANUAL_TEST_DIR)SpriteSheetAnimation_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# Paddle
PADDLE_TEST_SOURCES := $(MANUAL_TEST_DIR)Paddle_Tests.cpp \
                       $(SOURCE_DIR)Paddle.cpp \
                       $(SOURCE_DIR)GameObject.cpp \
                       $(SOURCE_DIR)GrowableObject.cpp \
                       $(SOURCE_DIR)Display.cpp \
                       $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                       $(SOURCE_DIR)TextureManager.cpp

PADDLE_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(PADDLE_TEST_SOURCES:.cpp=.o)))

paddle-test: $(PADDLE_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(PADDLE_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)paddle-test

$(BUILD_DIR)Paddle_Tests.o: $(MANUAL_TEST_DIR)Paddle_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# ThemeManager
THEME_TEST_SOURCES := $(MANUAL_TEST_DIR)ThemeManager_Tests.cpp \
                      $(SOURCE_DIR)ThemeManager.cpp \
                      $(SOURCE_DIR)ConfigFileManager.cpp

THEME_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(THEME_TEST_SOURCES:.cpp=.o)))

theme-test: $(THEME_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(THEME_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)theme-test

$(BUILD_DIR)ThemeManager_Tests.o: $(MANUAL_TEST_DIR)ThemeManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# TextManager
TEXT_TEST_SOURCES := $(MANUAL_TEST_DIR)TextManager_Tests.cpp \
                     $(SOURCE_DIR)TextManager.cpp \
                     $(SOURCE_DIR)Display.cpp \
                     $(SOURCE_DIR)TextureManager.cpp

TEXT_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TEXT_TEST_SOURCES:.cpp=.o)))

text-test: $(TEXT_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(TEXT_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)text-test

$(BUILD_DIR)TextManager_Tests.o: $(MANUAL_TEST_DIR)TextManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# SoundManager
SOUND_TEST_SOURCES := $(MANUAL_TEST_DIR)SoundManager_Tests.cpp \
                      $(SOURCE_DIR)SoundManager.cpp \
                      $(SOURCE_DIR)Display.cpp

SOUND_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOUND_TEST_SOURCES:.cpp=.o)))

sound-test: $(SOUND_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SOUND_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)sound-test

$(BUILD_DIR)SoundManager_Tests.o: $(MANUAL_TEST_DIR)SoundManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# Ball
BALL_TEST_SOURCES := $(MANUAL_TEST_DIR)Ball_Tests.cpp \
                     $(SOURCE_DIR)Ball.cpp \
                     $(SOURCE_DIR)Tracer.cpp \
                     $(SOURCE_DIR)Paddle.cpp \
                     $(SOURCE_DIR)GameObject.cpp \
                     $(SOURCE_DIR)GrowableObject.cpp \
                     $(SOURCE_DIR)Display.cpp \
                     $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                     $(SOURCE_DIR)TextureManager.cpp \
                     $(SOURCE_DIR)CollisionManager.cpp \
                     $(SOURCE_DIR)PlayfieldBorder.cpp \
                     $(SOURCE_DIR)EventManager.cpp

BALL_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(BALL_TEST_SOURCES:.cpp=.o)))

ball-test: $(BALL_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(BALL_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)ball-test

$(BUILD_DIR)CollisionManager.o: $(SOURCE_DIR)CollisionManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)PlayfieldBorder.o: $(SOURCE_DIR)PlayfieldBorder.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)EventManager.o: $(SOURCE_DIR)EventManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)Ball_Tests.o: $(MANUAL_TEST_DIR)Ball_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# EffectManager
EFFECT_TEST_SOURCES := $(MANUAL_TEST_DIR)EffectManager_Tests.cpp \
                       $(SOURCE_DIR)EffectManager.cpp \
                       $(SOURCE_DIR)EventManager.cpp \
                       $(SOURCE_DIR)MathHelper.cpp \
                       $(SOURCE_DIR)Display.cpp \
                       $(SOURCE_DIR)TextureManager.cpp \
                       $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                       $(SOURCE_DIR)TextManager.cpp \

EFFECT_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(EFFECT_TEST_SOURCES:.cpp=.o)))

effect-test: $(EFFECT_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(EFFECT_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)effect-test

$(BUILD_DIR)EffectManager_Tests.o: $(MANUAL_TEST_DIR)EffectManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# CLEANUP
clean:
	rm -rf $(BUILD_DIR)*.o $(BUILD_DIR)*-test $(BUILD_DIR)$(TARGET)


clean-tests:
	rm -rf $(BUILD_DIR)*-test $(BUILD_DIR)*_Tests.o

clean-all:
	rm -rf $(BUILD_DIR) 2>/dev/null || true
