# BackgroundManager_Todo.md

## open issues

- [ ] create test theme, use supported image-formats and different sizes
- [ ] test theme switching
- [ ] overlay off should respond immidiately and force recreation of display-list.
- [ ] tiled backgrounds | [see](../misc/Background.md)

## to be implemented

Gibt der TextureManager ein false zurück. lief irgendetwas schief und runtime-setting.showBg = false.
Beim nächsten Update wird der BackgroundManager nicht mehr gezeichnet. Aber nach dem Level wird BGManager
updateBgIfNeeded erneut versucht. Vielleicht gab es nur ein Problem mit einem Hintergrund.

Auflösung und Format sollten keine große Rolle, lediglich das AR von 4:3 ist wichtig.
4:3 kann auch einfach der Mitte genommen werden
4 colors for the background, will be set randomly

## closed issues

- [X] BackgroundManager_Tests
- [X] implement the tainting of the background, with a reasonable mix (blend) function and settings
- [X] bool showBgOverlay;
- [X] use mathHelper
- [X] use colors.h
- [X] use event-driven updates

## after json settings

? showbg=1 zu, setting.showBg = false; - later on I will use JSON and nested struct in settings anyway.

## after shader is working

- [ ] animated backgrounds (sprite sheet oder colors)
- [ ] make BackgroundManager-Shader responsive to events
