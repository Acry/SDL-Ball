# BackgroundManager_Todo.md

?
showbg=1 zu, setting.showBg = false;

Gibt der TextureManager ein false zurück. lief irgendetwas schief und runtime-setting.showBg = false.
Beim nächsten Update wird der BackgroundManager nicht mehr gezeichnet. Aber nach dem Level wird BGManager
updateBgIfNeeded erneut versucht. Vielleicht gab es nur ein Problem mit einem Hintergrund.

Auflösung und Format sollten keine große Rolle, lediglich das AR von 4:3 ist wichtig.
4:3 kann auch einfach der Mitte genommen werden
4 colors for the background, will be set randomly

- [X] BackgroundManager_Tests
- [ ] tiled backgrounds | [see](../misc/Background.md)
- [ ] animated backgrounds (spritsheet oder colors)
- [ ] make BackgroundManager-Shader responsive to events

## BG Overlay

- [ ] implement the tainting of the background, with a reasonable mix (blend) function and settings
- [ ] bool showBgOverlay;

Wir geben in den BG-Manager ein Setting, ob das Färben des Hintergrunds (original-Fassung) gewünscht ist.

Den Wert `showbgOverlay` fügen wir der Config zu.

Das war der alte Code:

```c++
for(int i = 0; i  < 4; i++)
{
    r[i] = rndflt(1,0);
    g[i] = rndflt(1,0);
    b[i] = rndflt(1,0);
}

a= 1;
```
