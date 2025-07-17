# make -j33 PREFIX=/usr THEME_DIRECTORY=/games/sdl-ball-themes/
PREFIX?=~/.local
THEME_DIRECTORY?=$(PREFIX)/share/sdl-ball-remastered/themes/
BINDIR?=$(PREFIX)/bin/

CXX ?= clang++

COMMON_FLAGS := -std=c++23 -Wall -Wextra -mtune=native $(shell sdl2-config --cflags)
LDFLAGS := -lepoxy $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
# Currently I use the local parent as data-dir during debug
DEBUG_FLAGS := -ggdb -gdwarf-5 -O0 -Wall -DDEBUG -fdebug-types-section -DTHEME_DIRECTORY="\"../\"" $(COMMON_FLAGS)
RELEASE_FLAGS := -O3 -DNDEBUG $(COMMON_FLAGS) -DTHEME_DIRECTORY="\"$(THEME_DIRECTORY)\""

# directories
BUILD_DIR := build/
SOURCE_DIR := src/
MANUAL_TEST_DIR := tests/manual/
AUTOMATIC_TEST_DIR := tests/automatic/

SOURCES := $(addprefix $(SOURCE_DIR), \
    BackgroundManager.cpp \
    Ball.cpp \
    BallManager.cpp \
    Brick.cpp \
    Clock.cpp \
    CodeManager.cpp \
    CollisionManager.cpp \
    ConfigFileManager.cpp \
    DisplayManager.cpp \
    EffectManager.cpp \
    EventDispatcher.cpp \
    EventManager.cpp \
    GrowableObject.cpp \
    HudManager.cpp \
    KeyboardManager.cpp \
    LevelManager.cpp \
    Lifes.cpp \
    MathHelper.cpp \
    MouseManager.cpp \
    Paddle.cpp \
    Particle.cpp \
    ParticleEffect.cpp \
    PlayfieldBorder.cpp \
    SaveGameManager.cpp \
    Score.cpp \
    SettingsManager.cpp \
    SoundManager.cpp \
    Speedometer.cpp \
    SplashScreen.cpp \
    SpriteSheetAnimation.cpp \
    SpriteSheetAnimationManager.cpp \
    TextManager.cpp \
    TextureManager.cpp \
    TextureUtilities.cpp \
    ThemeManager.cpp \
    Tracer.cpp \
    TransitionEffect.cpp \
    game_state.cpp \
    main.cpp \
)

# Create the build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOURCES:.cpp=.o)))

TEST_TARGETS := \
    test-background \
    test-ball \
    test-border \
    test-brick \
    test-code \
    test-collision \
    test-controller \
    test-config \
    test-display \
    test-effectManager \
    test-effects \
    test-game \
    test-hud \
    test-level \
    test-menu \
    test-paddle \
    test-scenes \
    test-settings \
    test-sound \
    test-splash \
    test-spritesheet \
    test-spritesheet-manager \
    test-text \
    test-texture \
    test-theme \
    test-tracer \

AUTO_TEST_TARGETS := \
    automatic-test-config \
    automatic-test-settings \

TARGET=sdl-ball-remastered

GAME_OBJECTS := $(OBJECTS)

.PHONY: all clean-all clean debug release install install-bin install-data game tests clean-tests

all: game tests autoTests

game: release debug

tests: $(TEST_TARGETS)

autoTests: $(AUTO_TEST_TARGETS)

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
	mkdir -p $(DESTDIR)$(THEME_DIRECTORY)
	cp -p -R themes/* $(DESTDIR)$(THEME_DIRECTORY)

remove-config:
	rm -R ~/.config/sdl-ball-remastered





###############################################################################
# MANUAL TEST TARGETS
# This section contains the manual test targets for the components of the project.
###############################################################################
# ControllerManagerTests
CONTROLLER_TEST_SOURCES := $(MANUAL_TEST_DIR)ControllerManager_Tests.cpp \

CONTROLLER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONTROLLER_TEST_SOURCES:.cpp=.o)))

test-controller: $(CONTROLLER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONTROLLER_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)test-controller

$(BUILD_DIR)ControllerManager_Tests.o: $(MANUAL_TEST_DIR)ControllerManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# ConfigManagerTests
CONFIG_TEST_SOURCES := $(MANUAL_TEST_DIR)ConfigFileManager_Tests.cpp \
                       $(SOURCE_DIR)ConfigFileManager.cpp \

CONFIG_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONFIG_TEST_SOURCES:.cpp=.o)))

test-config: $(CONFIG_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONFIG_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)test-config

$(BUILD_DIR)ConfigFileManager_Tests.o: $(MANUAL_TEST_DIR)ConfigFileManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

$(BUILD_DIR)ConfigFileManager.o: $(SOURCE_DIR)ConfigFileManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# SettingsManagerTests
SETTINGS_TEST_SOURCES := $(MANUAL_TEST_DIR)SettingsManager_Tests.cpp \
                         $(SOURCE_DIR)SettingsManager.cpp \

SETTINGS_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SETTINGS_TEST_SOURCES:.cpp=.o)))

test-settings: $(SETTINGS_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SETTINGS_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)test-settings

$(BUILD_DIR)SettingsManager_Tests.o: $(MANUAL_TEST_DIR)SettingsManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

$(BUILD_DIR)settings_manager.o: $(SOURCE_DIR)SettingsManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# DisplayManagerTests
DISPLAY_TEST_SOURCES := $(MANUAL_TEST_DIR)DisplayManager_Tests.cpp \
                        $(MANUAL_TEST_DIR)TestHelper.cpp \
                        $(SOURCE_DIR)DisplayManager.cpp \
                        $(SOURCE_DIR)TextManager.cpp \
                        $(SOURCE_DIR)TextureUtilities.cpp \
                        $(SOURCE_DIR)EventManager.cpp \
                        $(SOURCE_DIR)EventDispatcher.cpp \
                        $(SOURCE_DIR)MouseManager.cpp \

DISPLAY_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(DISPLAY_TEST_SOURCES:.cpp=.o)))

test-display: $(DISPLAY_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(DISPLAY_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-display

$(BUILD_DIR)DisplayManager_Tests.o: $(MANUAL_TEST_DIR)DisplayManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

TEST_HELPER_SOURCES := $(MANUAL_TEST_DIR)TestHelper.cpp \
                $(SOURCE_DIR)DisplayManager.cpp \

TestHelper: $(TEST_HELPER_SOURCES)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(TEST_HELPER_SOURCES) $(LDFLAGS) -o $(BUILD_DIR)TestHelper

$(BUILD_DIR)TestHelper.o: $(MANUAL_TEST_DIR)TestHelper.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@
###############################################################################
# BackgroundManagerTests
BACKGROUND_TEST_SOURCES := $(MANUAL_TEST_DIR)BackgroundManager_Tests.cpp \
                           $(MANUAL_TEST_DIR)TestHelper.cpp \
                           $(SOURCE_DIR)EventManager.cpp \
                           $(SOURCE_DIR)LevelManager.cpp \
                           $(SOURCE_DIR)DisplayManager.cpp \
                           $(SOURCE_DIR)MathHelper.cpp \
                           $(SOURCE_DIR)TextManager.cpp \
                           $(SOURCE_DIR)TextureManager.cpp \
                           $(SOURCE_DIR)TextureUtilities.cpp \
                           $(SOURCE_DIR)BackgroundManager.cpp \
                           $(SOURCE_DIR)MouseManager.cpp \
                           $(SOURCE_DIR)EventDispatcher.cpp \

BACKGROUND_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(BACKGROUND_TEST_SOURCES:.cpp=.o)))

test-background: $(BACKGROUND_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(BACKGROUND_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-background

$(BUILD_DIR)%.o: $(MANUAL_TEST_DIR)%.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

$(BUILD_DIR)%.o: $(SOURCE_DIR)%.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)Texture.o: $(SOURCE_DIR)SpriteSheetAnimation.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)TextureManager.o: $(SOURCE_DIR)TextureManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)BackgroundManager.o: $(SOURCE_DIR)BackgroundManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

###############################################################################
# SpriteSheetAnimationTests
SPRITESHEET_TEST_SOURCES := $(MANUAL_TEST_DIR)SpriteSheetAnimation_Tests.cpp \
                            $(MANUAL_TEST_DIR)TestHelper.cpp \
                            $(SOURCE_DIR)DisplayManager.cpp \
                            $(SOURCE_DIR)EventManager.cpp \
                            $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                            $(SOURCE_DIR)TextManager.cpp \
                            $(SOURCE_DIR)TextureManager.cpp \
                            $(SOURCE_DIR)TextureUtilities.cpp \

SPRITESHEET_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SPRITESHEET_TEST_SOURCES:.cpp=.o)))

test-spritesheet: $(SPRITESHEET_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(SPRITESHEET_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-spritesheet

$(BUILD_DIR)SpriteSheetAnimation_Tests.o: $(MANUAL_TEST_DIR)SpriteSheetAnimation_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# SpriteSheetAnimationManagerTests
SPRITESHEETMANAGER_TEST_SOURCES := $(MANUAL_TEST_DIR)SpriteSheetAnimationManager_Tests.cpp \
                                   $(MANUAL_TEST_DIR)TestHelper.cpp \
                                   $(SOURCE_DIR)DisplayManager.cpp \
                                   $(SOURCE_DIR)EventDispatcher.cpp \
                                   $(SOURCE_DIR)EventManager.cpp \
                                   $(SOURCE_DIR)MouseManager.cpp \
                                   $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                                   $(SOURCE_DIR)SpriteSheetAnimationManager.cpp \
                                   $(SOURCE_DIR)TextManager.cpp \
                                   $(SOURCE_DIR)TextureManager.cpp \
                                   $(SOURCE_DIR)TextureUtilities.cpp \

SPRITESHEETMANAGER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SPRITESHEETMANAGER_TEST_SOURCES:.cpp=.o)))

test-spritesheet-manager: $(SPRITESHEETMANAGER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(SPRITESHEETMANAGER_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-spritesheet-manager

$(BUILD_DIR)SpriteSheetAnimationManager_Tests.o: $(MANUAL_TEST_DIR)SpriteSheetAnimationManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# PaddleTests
PADDLE_TEST_SOURCES := $(MANUAL_TEST_DIR)Paddle_Tests.cpp \
                       $(MANUAL_TEST_DIR)TestHelper.cpp \
                       $(SOURCE_DIR)CollisionManager.cpp \
                       $(SOURCE_DIR)DisplayManager.cpp \
                       $(SOURCE_DIR)EventDispatcher.cpp \
                       $(SOURCE_DIR)EventManager.cpp \
                       $(SOURCE_DIR)GameObject.cpp \
                       $(SOURCE_DIR)GrowableObject.cpp \
                       $(SOURCE_DIR)MouseManager.cpp \
                       $(SOURCE_DIR)KeyboardManager.cpp \
                       $(SOURCE_DIR)MovingObject.cpp \
                       $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                       $(SOURCE_DIR)SpriteSheetAnimationManager.cpp \
                       $(SOURCE_DIR)TextManager.cpp \
                       $(SOURCE_DIR)TextureManager.cpp \
                       $(SOURCE_DIR)TextureUtilities.cpp \

PADDLE_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(PADDLE_TEST_SOURCES:.cpp=.o)))

test-paddle: $(PADDLE_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(PADDLE_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-paddle

$(BUILD_DIR)Paddle_Tests.o: $(MANUAL_TEST_DIR)Paddle_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# TracerTests
TRACER_TEST_SOURCES := $(MANUAL_TEST_DIR)Tracer_Tests.cpp \
                       $(MANUAL_TEST_DIR)TestHelper.cpp \
                       $(SOURCE_DIR)DisplayManager.cpp \
                       $(SOURCE_DIR)EventDispatcher.cpp \
                       $(SOURCE_DIR)EventManager.cpp \
                       $(SOURCE_DIR)GameObject.cpp \
                       $(SOURCE_DIR)KeyboardManager.cpp \
                       $(SOURCE_DIR)MathHelper.cpp \
                       $(SOURCE_DIR)MouseManager.cpp \
                       $(SOURCE_DIR)MovingObject.cpp \
                       $(SOURCE_DIR)Particle.cpp \
                       $(SOURCE_DIR)ParticleEffect.cpp \
                       $(SOURCE_DIR)TextManager.cpp \
                       $(SOURCE_DIR)TextureManager.cpp \
                       $(SOURCE_DIR)TextureUtilities.cpp \
                       $(SOURCE_DIR)Tracer.cpp \

TRACER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TRACER_TEST_SOURCES:.cpp=.o)))

test-tracer: $(TRACER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(TRACER_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-tracer

$(BUILD_DIR)Tracer_Tests.o: $(MANUAL_TEST_DIR)Tracer_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# EffectsTests
EFFECTS_TEST_SOURCES := $(MANUAL_TEST_DIR)Effects_Tests.cpp \
                        $(MANUAL_TEST_DIR)TestHelper.cpp \
                        $(SOURCE_DIR)DisplayManager.cpp \
                        $(SOURCE_DIR)EventDispatcher.cpp \
                        $(SOURCE_DIR)EventManager.cpp \
                        $(SOURCE_DIR)KeyboardManager.cpp \
                        $(SOURCE_DIR)MathHelper.cpp \
                        $(SOURCE_DIR)MouseManager.cpp \
                        $(SOURCE_DIR)Particle.cpp \
                        $(SOURCE_DIR)ParticleEffect.cpp \
                        $(SOURCE_DIR)TextManager.cpp \
                        $(SOURCE_DIR)TextureManager.cpp \
                        $(SOURCE_DIR)TextureUtilities.cpp \

EFFECTS_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(EFFECTS_TEST_SOURCES:.cpp=.o)))

test-effects: $(EFFECTS_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(EFFECTS_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-effects

$(BUILD_DIR)Effects_Tests.o: $(MANUAL_TEST_DIR)Effects_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# ScenesTests
SCENE_TEST_SOURCES := $(MANUAL_TEST_DIR)SceneManager_Tests.cpp \
                      $(MANUAL_TEST_DIR)TestHelper.cpp \
                      $(SOURCE_DIR)DisplayManager.cpp \
                      $(SOURCE_DIR)EventDispatcher.cpp \
                      $(SOURCE_DIR)EventManager.cpp \
                      $(SOURCE_DIR)KeyboardManager.cpp \
                      $(SOURCE_DIR)MathHelper.cpp \
                      $(SOURCE_DIR)MouseManager.cpp \
                      $(SOURCE_DIR)TextManager.cpp \
                      $(SOURCE_DIR)TextureManager.cpp \
                      $(SOURCE_DIR)TextureUtilities.cpp \
                      $(SOURCE_DIR)TransitionEffect.cpp \

SCENE_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SCENE_TEST_SOURCES:.cpp=.o)))

test-scenes: $(SCENE_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SCENE_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-scenes

$(BUILD_DIR)SceneManager_Tests.o: $(MANUAL_TEST_DIR)SceneManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# ThemeManagerTests
THEME_TEST_SOURCES := $(MANUAL_TEST_DIR)ThemeManager_Tests.cpp \
                      $(SOURCE_DIR)ThemeManager.cpp \
                      $(SOURCE_DIR)ConfigFileManager.cpp

THEME_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(THEME_TEST_SOURCES:.cpp=.o)))

test-theme: $(THEME_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(THEME_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)test-theme

$(BUILD_DIR)ThemeManager_Tests.o: $(MANUAL_TEST_DIR)ThemeManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# TextManagerTests
TEXT_TEST_SOURCES := $(MANUAL_TEST_DIR)TextManager_Tests.cpp \
                     $(MANUAL_TEST_DIR)TestHelper.cpp \
                     $(SOURCE_DIR)DisplayManager.cpp \
                     $(SOURCE_DIR)EventDispatcher.cpp \
                     $(SOURCE_DIR)EventManager.cpp \
                     $(SOURCE_DIR)KeyboardManager.cpp \
                     $(SOURCE_DIR)MouseManager.cpp \
                     $(SOURCE_DIR)TextManager.cpp \
                     $(SOURCE_DIR)TextureManager.cpp \
                     $(SOURCE_DIR)TextureUtilities.cpp \

TEXT_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TEXT_TEST_SOURCES:.cpp=.o)))

test-text: $(TEXT_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(TEXT_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-text

$(BUILD_DIR)TextManager_Tests.o: $(MANUAL_TEST_DIR)TextManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# SoundManagerTests
SOUND_TEST_SOURCES := $(MANUAL_TEST_DIR)SoundManager_Tests.cpp \
                      $(MANUAL_TEST_DIR)TestHelper.cpp \
                      $(SOURCE_DIR)DisplayManager.cpp \
                      $(SOURCE_DIR)EventManager.cpp \
                      $(SOURCE_DIR)SoundManager.cpp \
                      $(SOURCE_DIR)TextManager.cpp \
                      $(SOURCE_DIR)TextureUtilities.cpp \

SOUND_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOUND_TEST_SOURCES:.cpp=.o)))

test-sound: $(SOUND_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SOUND_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-sound

$(BUILD_DIR)SoundManager_Tests.o: $(MANUAL_TEST_DIR)SoundManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# SplashTests
SPLASH_TEST_SOURCES := $(MANUAL_TEST_DIR)SplashScene_Tests.cpp \
                      $(SOURCE_DIR)DisplayManager.cpp \
                      $(SOURCE_DIR)EventManager.cpp \
                      $(SOURCE_DIR)SoundManager.cpp \
                      $(SOURCE_DIR)SplashScreen.cpp \
                      $(SOURCE_DIR)TextureManager.cpp \
                      $(SOURCE_DIR)TextureUtilities.cpp \

SPLASH_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SPLASH_TEST_SOURCES:.cpp=.o)))

test-splash: $(SPLASH_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SPLASH_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-splash

$(BUILD_DIR)SplashScene_Tests.o: $(MANUAL_TEST_DIR)SplashScene_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# BallTests
BALL_TEST_SOURCES := $(MANUAL_TEST_DIR)Ball_Tests.cpp \
                     $(MANUAL_TEST_DIR)TestHelper.cpp \
                     $(SOURCE_DIR)Ball.cpp \
                     $(SOURCE_DIR)BallManager.cpp \
                     $(SOURCE_DIR)CollisionManager.cpp \
                     $(SOURCE_DIR)DisplayManager.cpp \
                     $(SOURCE_DIR)EventDispatcher.cpp \
                     $(SOURCE_DIR)EventManager.cpp \
                     $(SOURCE_DIR)GameObject.cpp \
                     $(SOURCE_DIR)GrowableObject.cpp \
                     $(SOURCE_DIR)MathHelper.cpp \
                     $(SOURCE_DIR)MouseManager.cpp \
                     $(SOURCE_DIR)MovingObject.cpp \
                     $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                     $(SOURCE_DIR)SpriteSheetAnimationManager.cpp \
                     $(SOURCE_DIR)TextManager.cpp \
                     $(SOURCE_DIR)TextureManager.cpp \
                     $(SOURCE_DIR)TextureUtilities.cpp \

BALL_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(BALL_TEST_SOURCES:.cpp=.o)))

test-ball: $(BALL_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(BALL_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-ball

$(BUILD_DIR)CollisionManager.o: $(SOURCE_DIR)CollisionManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)PlayfieldBorder.o: $(SOURCE_DIR)PlayfieldBorder.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)EventManager.o: $(SOURCE_DIR)EventManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)Ball_Tests.o: $(MANUAL_TEST_DIR)Ball_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# EffectManagerTests
EFFECT_MANAGER_TEST_SOURCES := $(MANUAL_TEST_DIR)EffectManager_Tests.cpp \
                              $(MANUAL_TEST_DIR)TestHelper.cpp \
                              $(SOURCE_DIR)DisplayManager.cpp \
                              $(SOURCE_DIR)EventDispatcher.cpp \
                              $(SOURCE_DIR)EventManager.cpp \
                              $(SOURCE_DIR)KeyboardManager.cpp \
                              $(SOURCE_DIR)MathHelper.cpp \
                              $(SOURCE_DIR)MouseManager.cpp \
                              $(SOURCE_DIR)Particle.cpp \
                              $(SOURCE_DIR)ParticleEffect.cpp \
                              $(SOURCE_DIR)TextManager.cpp \
                              $(SOURCE_DIR)TextureManager.cpp \
                              $(SOURCE_DIR)TextureUtilities.cpp \

EFFECT_MANAGER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(EFFECT_MANAGER_TEST_SOURCES:.cpp=.o)))

test-effectManager: $(EFFECT_MANAGER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(EFFECT_MANAGER_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-effectManager

$(BUILD_DIR)EffectManager_Tests.o: $(MANUAL_TEST_DIR)EffectManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# TextureManager - ThemesTests
TEXTURE_MANAGER_TEST_SOURCES := $(MANUAL_TEST_DIR)TextureManager_Tests.cpp \
                                $(MANUAL_TEST_DIR)TestHelper.cpp \
                                $(SOURCE_DIR)DisplayManager.cpp \
                                $(SOURCE_DIR)EventDispatcher.cpp \
                                $(SOURCE_DIR)EventManager.cpp \
                                $(SOURCE_DIR)MouseManager.cpp \
                                $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                                $(SOURCE_DIR)TextManager.cpp \
                                $(SOURCE_DIR)TextureManager.cpp \
                                $(SOURCE_DIR)TextureUtilities.cpp \

TEXTURE_MANAGER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TEXTURE_MANAGER_TEST_SOURCES:.cpp=.o)))

test-texture: $(TEXTURE_MANAGER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) -I$(SOURCE_DIR) $(TEXTURE_MANAGER_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-texture

$(BUILD_DIR)TextureManager_Tests.o: $(MANUAL_TEST_DIR)TextureManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# CollisionManagerTests
COLLISION_TEST_SOURCES := $(MANUAL_TEST_DIR)Collision_Tests.cpp \
                          $(MANUAL_TEST_DIR)TestHelper.cpp \
                          $(SOURCE_DIR)CollisionManager.cpp \
                          $(SOURCE_DIR)DisplayManager.cpp \
                          $(SOURCE_DIR)EventDispatcher.cpp \
                          $(SOURCE_DIR)EventManager.cpp \
                          $(SOURCE_DIR)KeyboardManager.cpp \
                          $(SOURCE_DIR)MouseManager.cpp \
                          $(SOURCE_DIR)TextManager.cpp \
                          $(SOURCE_DIR)TextureUtilities.cpp \

COLLISION_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(COLLISION_TEST_SOURCES:.cpp=.o)))

test-collision: $(COLLISION_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(COLLISION_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-collision

$(BUILD_DIR)Collision_Tests.o: $(MANUAL_TEST_DIR)Collision_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# MenuManagerTests
MENU_TEST_SOURCES := $(MANUAL_TEST_DIR)MenuManager_Tests.cpp \
                          $(MANUAL_TEST_DIR)TestHelper.cpp \
                          $(SOURCE_DIR)CollisionManager.cpp \
                          $(SOURCE_DIR)DisplayManager.cpp \
                          $(SOURCE_DIR)EventDispatcher.cpp \
                          $(SOURCE_DIR)EventManager.cpp \
                          $(SOURCE_DIR)KeyboardManager.cpp \
                          $(SOURCE_DIR)MouseManager.cpp \
                          $(SOURCE_DIR)TextManager.cpp \
                          $(SOURCE_DIR)TextureUtilities.cpp \

MENU_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(MENU_TEST_SOURCES:.cpp=.o)))

test-menu: $(MENU_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(MENU_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-menu

$(BUILD_DIR)MenuManager_Tests.o: $(MANUAL_TEST_DIR)MenuManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# LevelManagerTests
LEVEL_TEST_SOURCES := $(MANUAL_TEST_DIR)LevelManager_Tests.cpp \
                      $(MANUAL_TEST_DIR)TestHelper.cpp \
                      $(SOURCE_DIR)DisplayManager.cpp \
                      $(SOURCE_DIR)EventDispatcher.cpp \
                      $(SOURCE_DIR)EventManager.cpp \
                      $(SOURCE_DIR)LevelManager.cpp \
                      $(SOURCE_DIR)MouseManager.cpp \
                      $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                      $(SOURCE_DIR)TextManager.cpp \
                      $(SOURCE_DIR)TextureManager.cpp \
                      $(SOURCE_DIR)TextureUtilities.cpp \

LEVEL_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(LEVEL_TEST_SOURCES:.cpp=.o)))

test-level: $(LEVEL_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(LEVEL_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-level

$(BUILD_DIR)LevelManager_Tests.o: $(MANUAL_TEST_DIR)LevelManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# BricksManagerTests
BRICK_TEST_SOURCES := $(MANUAL_TEST_DIR)BrickManager_Tests.cpp \
                      $(MANUAL_TEST_DIR)TestHelper.cpp \
                      $(SOURCE_DIR)Brick.cpp \
                      $(SOURCE_DIR)BrickManager.cpp \
                      $(SOURCE_DIR)DisplayManager.cpp \
                      $(SOURCE_DIR)EventDispatcher.cpp \
                      $(SOURCE_DIR)EventManager.cpp \
                      $(SOURCE_DIR)GameObject.cpp \
                      $(SOURCE_DIR)LevelManager.cpp \
                      $(SOURCE_DIR)MouseManager.cpp \
                      $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                      $(SOURCE_DIR)SpriteSheetAnimationManager.cpp \
                      $(SOURCE_DIR)TextManager.cpp \
                      $(SOURCE_DIR)TextureManager.cpp \
                      $(SOURCE_DIR)TextureUtilities.cpp \

BRICK_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(BRICK_TEST_SOURCES:.cpp=.o)))

test-brick: $(BRICK_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(BRICK_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-brick

$(BUILD_DIR)BrickManager_Tests.o: $(MANUAL_TEST_DIR)BrickManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# BorderTests
BORDER_TEST_SOURCES := $(MANUAL_TEST_DIR)PlayfieldBorder_Tests.cpp \
                       $(MANUAL_TEST_DIR)TestHelper.cpp \
                       $(SOURCE_DIR)CollisionManager.cpp \
                       $(SOURCE_DIR)DisplayManager.cpp \
                       $(SOURCE_DIR)EventDispatcher.cpp \
                       $(SOURCE_DIR)EventManager.cpp \
                       $(SOURCE_DIR)KeyboardManager.cpp \
                       $(SOURCE_DIR)MouseManager.cpp \
                       $(SOURCE_DIR)PlayfieldBorder.cpp \
                       $(SOURCE_DIR)TextManager.cpp \
                       $(SOURCE_DIR)TextureManager.cpp \
                       $(SOURCE_DIR)TextureUtilities.cpp \

BORDER_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(BORDER_TEST_SOURCES:.cpp=.o)))

test-border: $(BORDER_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(BORDER_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-border

$(BUILD_DIR)PlayfieldBorder_Tests.o: $(MANUAL_TEST_DIR)PlayfieldBorder_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# Hud Tests
HUD_TEST_SOURCES := $(MANUAL_TEST_DIR)HudManager_Tests.cpp \
                    $(MANUAL_TEST_DIR)TestHelper.cpp \
                    $(SOURCE_DIR)Clock.cpp \
                    $(SOURCE_DIR)DisplayManager.cpp \
                    $(SOURCE_DIR)EventDispatcher.cpp \
                    $(SOURCE_DIR)EventManager.cpp \
                    $(SOURCE_DIR)HudManager.cpp \
                    $(SOURCE_DIR)KeyboardManager.cpp \
                    $(SOURCE_DIR)Lifes.cpp \
                    $(SOURCE_DIR)MouseManager.cpp \
                    $(SOURCE_DIR)Score.cpp \
                    $(SOURCE_DIR)TextManager.cpp \
                    $(SOURCE_DIR)TextureManager.cpp \
                    $(SOURCE_DIR)TextureUtilities.cpp \

HUD_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(HUD_TEST_SOURCES:.cpp=.o)))

test-hud: $(HUD_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(HUD_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-hud

$(BUILD_DIR)HudManager_Tests.o: $(MANUAL_TEST_DIR)HudManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# GameManager
GAME_TEST_SOURCES := $(MANUAL_TEST_DIR)GameManager_Tests.cpp \
                     $(MANUAL_TEST_DIR)TestHelper.cpp \
                     $(SOURCE_DIR)BackgroundManager.cpp \
                     $(SOURCE_DIR)Brick.cpp \
                     $(SOURCE_DIR)BrickManager.cpp \
                     $(SOURCE_DIR)Clock.cpp \
                     $(SOURCE_DIR)CodeManager.cpp \
                     $(SOURCE_DIR)ConfigFileManager.cpp \
                     $(SOURCE_DIR)DisplayManager.cpp \
                     $(SOURCE_DIR)EventDispatcher.cpp \
                     $(SOURCE_DIR)EventManager.cpp \
                     $(SOURCE_DIR)HudManager.cpp \
                     $(SOURCE_DIR)KeyboardManager.cpp \
                     $(SOURCE_DIR)LevelManager.cpp \
                     $(SOURCE_DIR)Lifes.cpp \
                     $(SOURCE_DIR)MathHelper.cpp \
                     $(SOURCE_DIR)MouseManager.cpp \
                     $(SOURCE_DIR)PlayfieldBorder.cpp \
                     $(SOURCE_DIR)Score.cpp \
                     $(SOURCE_DIR)SettingsManager.cpp \
                     $(SOURCE_DIR)SoundManager.cpp \
                     $(SOURCE_DIR)SplashScreen.cpp \
                     $(SOURCE_DIR)SpriteSheetAnimation.cpp \
                     $(SOURCE_DIR)SpriteSheetAnimationManager.cpp \
                     $(SOURCE_DIR)TextManager.cpp \
                     $(SOURCE_DIR)TextureManager.cpp \
                     $(SOURCE_DIR)TextureUtilities.cpp \
                     $(SOURCE_DIR)ThemeManager.cpp \

GAME_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(GAME_TEST_SOURCES:.cpp=.o)))

test-game: $(GAME_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(GAME_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-game

$(BUILD_DIR)GameManager_Tests.o: $(MANUAL_TEST_DIR)GameManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

###############################################################################
# CodeManager
CODE_TEST_SOURCES := $(MANUAL_TEST_DIR)CodeManager_Tests.cpp \
                       $(SOURCE_DIR)BackgroundManager.cpp \
                       $(SOURCE_DIR)CodeManager.cpp \
                       $(SOURCE_DIR)ConfigFileManager.cpp \
                       $(SOURCE_DIR)DisplayManager.cpp \
                       $(SOURCE_DIR)EventDispatcher.cpp \
                       $(SOURCE_DIR)EventManager.cpp \
                       $(SOURCE_DIR)KeyboardManager.cpp \
                       $(SOURCE_DIR)MathHelper.cpp \
                       $(SOURCE_DIR)MouseManager.cpp \
                       $(SOURCE_DIR)SettingsManager.cpp \
                       $(SOURCE_DIR)SoundManager.cpp \
                       $(SOURCE_DIR)SplashScreen.cpp \
                       $(SOURCE_DIR)TextManager.cpp \
                       $(SOURCE_DIR)TextureManager.cpp \
                       $(SOURCE_DIR)TextureUtilities.cpp \
                       $(SOURCE_DIR)ThemeManager.cpp \

CODE_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CODE_TEST_SOURCES:.cpp=.o)))

test-code: $(CODE_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CODE_TEST_OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)test-code

$(BUILD_DIR)CodeManager_Tests.o: $(MANUAL_TEST_DIR)CodeManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@

# END
###############################################################################

###############################################################################
# CLEANUP
###############################################################################
clean:
	rm -rf $(BUILD_DIR)*.o

clean-tests:
	rm -rf $(BUILD_DIR)test-*

clean-all:
	rm -rf $(BUILD_DIR) 2>/dev/null || true

# END
###############################################################################

###############################################################################
# AUTOMATIC TESTS - GTEST
###############################################################################
# automatic-test-texture
TEXTURE_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)TextureManager_aTests.cpp \
                         $(SOURCE_DIR)TextureUtilities.cpp \
                         $(SOURCE_DIR)TextureManager.cpp \
                         $(SOURCE_DIR)SpriteSheetAnimation.cpp

TEXTURE_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(TEXTURE_ATEST_SOURCES:.cpp=.o)))

texture-automatic-test: $(TEXTURE_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(TEXTURE_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)texture-automatic-test

$(BUILD_DIR)TextureManager_aTests.o: $(AUTOMATIC_TEST_DIR)TextureManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

###############################################################################
# automatic-test-event
EVENT_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)EventManager_aTests.cpp \
                        $(SOURCE_DIR)EventManager.cpp

EVENT_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(EVENT_ATEST_SOURCES:.cpp=.o)))

automatic-test-event: $(EVENT_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(EVENT_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)automatic-test-event

$(BUILD_DIR)EventManager_aTests.o: $(AUTOMATIC_TEST_DIR)EventManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

###############################################################################
# automatic-test-collision
COLLISION_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)CollisionManager_aTests.cpp \
                          $(SOURCE_DIR)CollisionManager.cpp

COLLISION_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(COLLISION_ATEST_SOURCES:.cpp=.o)))

automatic-test-collision: $(COLLISION_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(COLLISION_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)automatic-test-collision

$(BUILD_DIR)CollisionManager_aTests.o: $(AUTOMATIC_TEST_DIR)CollisionManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

###############################################################################
# automatic-test-level
LEVEL_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)LevelManager_aTests.cpp \
                       $(SOURCE_DIR)LevelManager.cpp

LEVEL_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(LEVEL_ATEST_SOURCES:.cpp=.o)))

automatic-test-level: $(LEVEL_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(LEVEL_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)automatic-test-level

$(BUILD_DIR)LevelManager_aTests.o: $(AUTOMATIC_TEST_DIR)LevelManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

###############################################################################
# automatic-test-config
CONFIG_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)ConfigFileManager_aTests.cpp \
                        $(SOURCE_DIR)ConfigFileManager.cpp \

CONFIG_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONFIG_ATEST_SOURCES:.cpp=.o)))

automatic-test-config: $(CONFIG_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONFIG_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)automatic-test-config

$(BUILD_DIR)ConfigFileManager_aTests.o: $(AUTOMATIC_TEST_DIR)ConfigFileManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

###############################################################################
# automatic-test-settings
SETTING_ATEST_SOURCES := $(AUTOMATIC_TEST_DIR)SettingsManager_aTests.cpp \
                        $(SOURCE_DIR)SettingsManager.cpp \

SETTING_ATEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SETTING_ATEST_SOURCES:.cpp=.o)))

automatic-test-settings: $(SETTING_ATEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(SETTING_ATEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -pthread -o $(BUILD_DIR)automatic-test-settings

$(BUILD_DIR)SettingsManager_aTests.o: $(AUTOMATIC_TEST_DIR)SettingsManager_aTests.cpp
	$(CXX) -c $(DEBUG_FLAGS) -I$(SOURCE_DIR) $< -o $@ -lgtest -lgtest_main -pthread

# END
###############################################################################
