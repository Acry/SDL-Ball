# Makefile

Die Kompilierreihenfolge sollte so aussehen, dass zuerst die Header-Dateien und dann die zugehörigen CPP-Dateien kompiliert werden. Hier ein Vorschlag für das Makefile:

```makefile
# Header-Dateien (Abhängigkeiten)
HEADERS = src/display.hpp \
          src/texture.hpp \
          src/menu.hpp

# Objekt-Dateien
OBJECTS = src/display.o \
          src/texture.o \
          src/menu.o

# Regeln für die Kompilierung
src/display.o: src/core/display.cpp src/core/display.hpp
	$(CC) $(CFLAGS) -c src/display.cpp -o src/display.o

src/texture.o: src/texture.cpp src/texture.hpp
	$(CC) $(CFLAGS) -c src/texture.cpp -o src/texture.o

src/menu.o: src/ui/menu.cpp src/ui/menu.hpp src/core/display.hpp src/texture.hpp
	$(CC) $(CFLAGS) -c src/menu.cpp -o src/menu.o
```

Die wichtigsten Punkte:
1. `display.hpp` zuerst, da es von anderen benötigt wird
2. `texture.hpp` als nächstes, da es von menu benötigt wird
3. `menu.hpp` zuletzt, da es die anderen Header einbindet
4. Dann die entsprechenden `.cpp` Dateien in gleicher Reihenfolge

Die Abhängigkeiten sind:
- `menu.cpp` braucht `menu.hpp`, `display.hpp`, `texture.hpp`
- `texture.cpp` braucht `texture.hpp`
- `display.cpp` braucht `display.hpp`
