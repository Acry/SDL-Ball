# Borders

// Am Anfang (z.B. beim Initialisieren)

```c++
struct BorderVertex {
GLfloat x, y, z;
GLfloat u, v;
};

GLuint borderVAO = 0, borderVBO = 0;
```

```c++
void createPlayfieldBorderVAO(const textureClass &tex) {
// Vertices für die beiden Border-Quads (links und rechts)
BorderVertex borderVertices[] = {
    // Left quad
    { -1.66f,  1.25f, 0.0f, 0.0f,  0.0f },
    { -1.60f,  1.25f, 0.0f, 1.0f,  0.0f },
    { -1.60f, -1.25f, 0.0f, 1.0f, -1.0f },
    { -1.66f, -1.25f, 0.0f, 0.0f, -1.0f },

    // Right quad
    {  1.66f,  1.25f, 0.0f, 0.0f,  0.0f },
    {  1.60f,  1.25f, 0.0f, 1.0f,  0.0f },
    {  1.60f, -1.25f, 0.0f, 1.0f, -1.0f },
    {  1.66f, -1.25f, 0.0f, 0.0f, -1.0f }
    };

    glGenVertexArrays(1, &borderVAO);
    glGenBuffers(1, &borderVBO);

    glBindVertexArray(borderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(borderVertices), borderVertices, GL_STATIC_DRAW);

    // Position
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(BorderVertex), (void*)offsetof(BorderVertex, x));
    // TexCoord
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(BorderVertex), (void*)offsetof(BorderVertex, u));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
```

// Zum Rendern (z.B. in draw-Funktion)

```c++
void drawPlayfieldBorder(const textureClass &tex) {
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, tex.prop.texture);
glColor4f(GL_WHITE);

    glBindVertexArray(borderVAO);
    // Zwei Quads = 8 Vertices, also 2x glDrawArrays mit je 4 Vertices oder einmal mit 8
    glDrawArrays(GL_QUADS, 0, 8);
    glBindVertexArray(0);
}
```
