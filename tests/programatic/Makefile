CXX = clang++
CXXFLAGS = -std=c++23 -Wall -Wextra -mtune=native $(shell sdl2-config --cflags)
INCLUDES = -I./include

# Quelldateien
SRCS = level_test.cpp ../src/loadlevel.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = level_test

# Build-Regeln
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Aufräumen
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
