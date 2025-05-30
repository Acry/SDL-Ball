# TextManager

fonts.txt
menufile=subgamefont.ttf
menusize=30

size is enough

glfontstash for the font atals would be a simple solution

## Kerning

// Im write-Loop für fortgeschrittenes Layout
if (i > 0) {
int kerning = TTF_GetFontKerningSizeGlyphs(ttfFont, text[i-1], text[i]);
if (kerning != -1) {
drawPosX += kerning / static_cast<float>(surfaceDim);
}
}
