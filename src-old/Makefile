# Arch Linux
PREFIX?=/usr

# Arch Linux
DATADIR?=$(PREFIX)/share/sdl-ball/themes/
BINDIR?=$(PREFIX)/bin/

#append -DWITH_WIIUSE to compile with WIIUSE support!
#append -DNOSOUND to compile WITHOUT sound support

CXX?=g++

CXXFLAGS+=-Wall `sdl-config --cflags` -DDATADIR="\"$(DATADIR)\""

#append -lwiiuse to compile with WIIUSE support
#remove -lSDL_mixer if compiling with -DNOSOUND
LIBS+=-lGL -lGLU `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer

SOURCES=old_main.cpp old_display.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=sdl-ball

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o sdl-ball

install: $(EXECUTABLE) install-bin install-data

install-bin:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(EXECUTABLE) $(DESTDIR)$(BINDIR)

install-data:
	mkdir -p $(DESTDIR)$(DATADIR)
	cp -p -R themes/* $(DESTDIR)$(DATADIR)

remove:
	rm -R ~/.config/sdl-ball
