PREFIX?=~/.local

DATADIR?=$(PREFIX)share/sdl-ball/themes/
BINDIR?=$(PREFIX)/bin/

CXX=clang++

COMMON_FLAGS := -std=c++23 -Wall -Wextra -mtune=native $(shell sdl2-config --cflags)
LDFLAGS := -lepoxy -lGLU  $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
DEBUG_FLAGS := -ggdb -gdwarf-5 -O0 -Wall -DDEBUG -fdebug-types-section -DDATADIR="\"./../\"" $(COMMON_FLAGS)
RELEASE_FLAGS := -O3 -DNDEBUG $(COMMON_FLAGS)

# directories
BUILD_DIR := build/
SOURCE_DIR := src/

SOURCES := $(addprefix $(SOURCE_DIR), Display.cpp main.cpp ConfigFileManager.cpp SettingsManager.cpp Text.cpp SaveFileManager.cpp SoundManager.cpp ThemeManager.cpp Texture.cpp game_state.cpp Score.cpp Background.cpp TextureManager.cpp Speedometer.cpp)
OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(SOURCES:.cpp=.o)))

# Create the build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

TARGET=sdl-ball

.PHONY: all clean debug release install install-bin install-data remove

all: release  debug

release: $(BUILD_DIR)$(TARGET)

debug: $(OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(OBJECTS) $(LDFLAGS) -o $(BUILD_DIR)$(TARGET)

$(BUILD_DIR)%.o: $(SOURCE_DIR)%.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) 2>/dev/null || true

install: $(BUILD_DIR)$(TARGET) install-bin install-data

install-bin:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BUILD_DIR)$(TARGET) $(DESTDIR)$(BINDIR)

install-data:
	mkdir -p $(DESTDIR)$(DATADIR)
	cp -p -R themes/* $(DESTDIR)$(DATADIR)

remove:
	rm -R ~/.config/sdl-ball

# Test targets
CONFIG_TEST_SOURCES := $(SOURCE_DIR)config_file_test.cpp \
                      $(SOURCE_DIR)config_file.cpp \
                      $(SOURCE_DIR)settings_manager.cpp

CONFIG_TEST_OBJECTS := $(addprefix $(BUILD_DIR), $(notdir $(CONFIG_TEST_SOURCES:.cpp=.o)))

config-test: $(CONFIG_TEST_OBJECTS)
	$(CXX) $(DEBUG_FLAGS) $(CONFIG_TEST_OBJECTS) $(shell sdl2-config --libs) -o $(BUILD_DIR)config-test

# Spezielle Regel für die Test-Objekte
$(BUILD_DIR)config_file_test.o: $(SOURCE_DIR)ConfigFileManager_Tests.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)config_file.o: $(SOURCE_DIR)ConfigFileManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@

$(BUILD_DIR)settings_manager.o: $(SOURCE_DIR)SettingsManager.cpp
	$(CXX) -c $(DEBUG_FLAGS) $< -o $@
