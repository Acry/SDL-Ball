# TextManager

fonts.txt
menufile=subgamefont.ttf
menusize=30

size is enough

glfontstash for the font atals would be a simple solution

___

// In der write-Methode
GLfloat baseline = fontInfo[fontIndex].ascent; // Position der Baseline

glBegin(GL_QUADS);
// Achte darauf, dass die Zeichen an der Baseline ausgerichtet werden, nicht am oberen oder unteren Rand
glTexCoord2f(fontInfo[fontIndex].ch[c].uLeft, fontInfo[fontIndex].ch[c].vBottom);
glVertex3f(-sX + drawPosX, -baseline, 0.0f); // Baseline als Referenzpunkt
// ... weitere Eckpunkte entsprechend anpassen

// Im write-Loop für fortgeschrittenes Layout
if (i > 0) {
int kerning = TTF_GetFontKerningSizeGlyphs(ttfFont, text[i-1], text[i]);
if (kerning != -1) {
drawPosX += kerning / static_cast<float>(surfaceDim);
}
}
