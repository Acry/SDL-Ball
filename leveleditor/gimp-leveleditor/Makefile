CC = gcc
CFLAGS = `pkg-config --cflags gimp-3.0 gimpui-3.0`
LDFLAGS = `pkg-config --libs gimp-3.0 gimpui-3.0`

gimp-sdlball3: gimp-sdlball.c
	$(CC) $(CFLAGS) -o gimp-sdlball3 gimp-sdlball3.1.c $(LDFLAGS)
	mkdir -p ~/.config/GIMP/3.0/plug-ins/gimp-sdlball3/
	cp gimp-sdlball3 ~/.config/GIMP/3.0/plug-ins/gimp-sdlball3/

.PHONY: clean
clean:
	rm -f gimp-sdlball3.1