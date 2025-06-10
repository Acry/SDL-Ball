# TextManager

## Font Atlas

glfontstash for the font atlas would be a simple solution, but it is depreacted and I don't want to use it. Managing an
own atlas is a good experience.

## Kerning

Blocker: FontAtlas

```c++
// Im write-Loop für fortgeschrittenes Layout
if (i > 0) {
    int kerning = TTF_GetFontKerningSizeGlyphs(ttfFont, text[i-1], text[i]);
    if (kerning != -1) {
        drawPosX += kerning / static_cast<float>(surfaceDim);
    }
}
```
