# BackgroundManager_Todo.md

## See: FIXME: BackgroundManager_Todo.md

[BackgroundManager_Todo](../misc/BackgroundManager_Todo.md)

[ ] Overlay to setting
[ ] Background-Texture to setting

## to be implemented

Gibt der TextureManager ein false zurück. lief irgendetwas schief und runtime-setting.showBg = false.
Beim nächsten Update wird der BackgroundManager nicht mehr gezeichnet. Aber nach dem Level wird BGManager
updateBgIfNeeded erneut versucht. Vielleicht gab es nur ein Problem mit einem Hintergrund.

Auflösung und Format sollten keine große Rolle, lediglich das AR von 4:3 ist wichtig.
4:3 kann auch einfach der Mitte genommen werden
4 colors for the background, will be set randomly

- [ ] color layer could repsond to collsions or music/sound
- [ ] tiled backgrounds | [see](../misc/Background.md)
- [ ] AspectRatioin handling
- [ ] Rethink: Use Type SpriteSheetAnimation for backgrounds?
- [ ] Background-Texture from TextureManager?

## after json settings

? showbg=1 zu, setting.showBg = false; - later on I will use JSON and nested struct in settings anyway.

## after shader is working

- [ ] animated backgrounds (sprite sheet oder colors)
- [ ] make BackgroundManager-Shader responsive to events or Background Music

## Modernisierung des BackgroundManagers mit Shadern

Blocker: ShaderManager: Vertex- und Fragment-Shader sollten für lange Zeit reichen.

Der BackgroundManager ist ein idealer Kandidat für die Einführung von Shadern!
Er hat eine einfache Aufgabe (Textur-Rendering auf einem Quad) und verwendet derzeit Display-Listen, die in modernem
OpenGL veraltet sind.

### 1. Schrittweise Modernisierung mit Compatibility Profile

Das OpenGL 3.1 Compatibility Profile kann sowohl Shader als auch legacy-Code gemeinsam nutzen. So könntest du den
BackgroundManager modernisieren, ohne andere Teile des Spiels zu beeinträchtigen.

Hier ist ein modernisierter BackgroundManager mit Shadern:

```cpp
class BackgroundManager {
private:
    // Shader-bezogene Daten
    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    
    // Alte Daten für Fallback/Kompatibilität
    GLuint backgroundDisplayList;
    
    // Farben und Transparenz
    GLfloat r[4], g[4], b[4], a;
    
    const TextureManager* textureMgr;
    SpriteSheetAnimation tex;
    
    // Neue Methoden für Shader-Setup
    void setupShaders();
    void setupBuffers();
    
    // Legacy-Methode
    void drawQuad() const;
    
    // Flag für Verwendung von Shadern
    bool useShaders;

public:
    explicit BackgroundManager(const TextureManager &texMgr);
    ~BackgroundManager();

    bool updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName);
    void draw() const;
};
```

### 2. Implementierung mit Shadern

Hier ist die Implementierung des modernisierten BackgroundManagers:

```cpp
#include "BackgroundManager.h"
#include "TextureManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Vertex-Shader
const char* vertexShaderSource = R"(
    #version 150
    in vec3 position;
    in vec2 texCoord;
    in vec4 color;
    
    out vec2 fragTexCoord;
    out vec4 fragColor;
    
    uniform mat4 transform;
    
    void main() {
        gl_Position = transform * vec4(position, 1.0);
        fragTexCoord = texCoord;
        fragColor = color;
    }
)";

// Fragment-Shader
const char* fragmentShaderSource = R"(
    #version 150
    in vec2 fragTexCoord;
    in vec4 fragColor;
    
    out vec4 outColor;
    
    uniform sampler2D texSampler;
    uniform float alpha;
    
    void main() {
        vec4 texColor = texture(texSampler, fragTexCoord);
        outColor = texColor * fragColor * vec4(1.0, 1.0, 1.0, alpha);
    }
)";

BackgroundManager::BackgroundManager(const TextureManager &texMgr) : 
    backgroundDisplayList(0),
    shaderProgram(0),
    VAO(0),
    VBO(0),
    EBO(0),
    textureMgr(&texMgr) 
{
    // Shader-Unterstützung prüfen
    useShaders = glewIsSupported("GL_VERSION_3_1") != 0;
    
    // Farben initialisieren
    for (int i = 0; i < 4; i++) {
        r[i] = 0.9f;
        g[i] = 0.9f;
        b[i] = 0.9f;
    }
    a = 0.95f;
    
    if (useShaders) {
        setupShaders();
        setupBuffers();
    }
}

void BackgroundManager::setupShaders() {
    // Vertex-Shader kompilieren
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    // Fehlerprüfung
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex-Shader-Kompilierungsfehler:\n" << infoLog << std::endl;
        useShaders = false;
        return;
    }
    
    // Fragment-Shader kompilieren
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Fehlerprüfung
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment-Shader-Kompilierungsfehler:\n" << infoLog << std::endl;
        useShaders = false;
        return;
    }
    
    // Shader-Programm erstellen und verlinken
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Fehlerprüfung
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader-Programm-Linkfehler:\n" << infoLog << std::endl;
        useShaders = false;
        return;
    }
    
    // Shader-Objekte nach dem Linken löschen
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void BackgroundManager::setupBuffers() {
    // Vertexdaten für ein Quad
    float vertices[] = {
        // Position (x,y,z)    // Texturkoordinaten  // Farben (r,g,b,a)
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,           r[0], g[0], b[0], 1.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f,           r[1], g[1], b[1], 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f,           r[2], g[2], b[2], 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,           r[3], g[3], b[3], 1.0f
    };
    
    // Indices für EBO
    unsigned int indices[] = {
        0, 1, 2,   // Erstes Dreieck
        0, 2, 3    // Zweites Dreieck
    };
    
    // VAO erstellen und binden
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // VBO erstellen und Daten hochladen
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // EBO erstellen und Daten hochladen
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Vertex-Attribute konfigurieren
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texturkoordinaten
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Farbe
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Unbinden
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool BackgroundManager::updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName) {
    const int bgNumber = level * 25.0f / 50.0f + 1;
    bool result;

    if (bgNumber > 25) {
        const std::filesystem::path finalPath = pathName / "final.jpg";
        result = textureMgr->load(finalPath, tex);
    } else {
        const std::filesystem::path bgPath = pathName / (std::to_string(bgNumber) + ".jpg");
        result = textureMgr->load(bgPath, tex);
    }
    
    if (result) {
        // Legacy-Pfad für Kompatibilität
        if (backgroundDisplayList != 0) {
            glDeleteLists(backgroundDisplayList, 1);
        }
        backgroundDisplayList = glGenLists(1);
        glNewList(backgroundDisplayList, GL_COMPILE);
        drawQuad();
        glEndList();
        
        // Wenn VBO existiert, Vertex-Daten aktualisieren
        if (useShaders && VBO != 0) {
            // Bei einer Texturänderung muss VBO nicht aktualisiert werden,
            // da wir nur die Textur-ID im Shader ändern
        }
    }
    
    return result;
}

void BackgroundManager::draw() const {
    if (useShaders && shaderProgram != 0 && VAO != 0) {
        // Moderner Rendering-Pfad mit Shadern
        glUseProgram(shaderProgram);
        
        // Textur binden
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
        
        // Uniforms setzen
        glUniform1i(glGetUniformLocation(shaderProgram, "texSampler"), 0);
        glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), a);
        
        // Transformationsmatrix (in diesem Fall Identität)
        glm::mat4 transform = glm::mat4(1.0f);
        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "transform"),
            1, GL_FALSE, glm::value_ptr(transform)
        );
        
        // Alpha-Blending aktivieren
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Zeichnen
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Aufräumen
        glUseProgram(0);
        glDisable(GL_BLEND);
    } else if (backgroundDisplayList != 0) {
        // Legacy-Pfad mit Display-Listen
        glCallList(backgroundDisplayList);
    }
}

void BackgroundManager::drawQuad() const {
    // Legacy-Methode für Display-Listen, unverändert
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

BackgroundManager::~BackgroundManager() {
    // Display-Listen aufräumen
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
    
    // Shader-Ressourcen aufräumen
    if (shaderProgram != 0) {
        glDeleteProgram(shaderProgram);
    }
    
    // Buffer-Objekte aufräumen
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
    }
}
```

### 3. Vorteile dieser Implementierung

1. **Fallback-Mechanismus**: Wenn Shader nicht unterstützt werden oder ein Fehler auftritt, werden automatisch
   Display-Listen verwendet
2. **Kompatibilität**: Funktioniert sowohl mit modernem als auch mit Legacy-OpenGL
3. **Performance**: VBOs und VAOs sind deutlich performanter als Display-Listen
4. **Erweiterbarkeit**: Der Shader-Code kann leicht um weitere Effekte erweitert werden
5. **Sauberer Code**: Klare Trennung zwischen modernem und Legacy-Code

### 4. Mögliche Erweiterungen

Mit diesem Grundgerüst könntest du den Hintergrund noch weiter aufwerten:

#### a) Animierte Hintergründe

```glsl
// Im Fragment-Shader
uniform float time;// Zeit in Sekunden

void main() {
    // Wellenbewegung für Texturkoordinaten
    vec2 movedTexCoords = fragTexCoord;
    movedTexCoords.y += sin(fragTexCoord.x * 10.0 + time) * 0.02;

    vec4 texColor = texture(texSampler, movedTexCoords);
    outColor = texColor * fragColor * vec4(1.0, 1.0, 1.0, alpha);
}
```

#### b) Vignette-Effekt -> EffectManager

```glsl
void main() {
    vec2 uv = fragTexCoord;
    vec4 texColor = texture(texSampler, uv);

    // Vignette-Effekt
    float vignette = 1.0 - length((uv - 0.5) * 1.5);
    vignette = clamp(vignette, 0.0, 1.0);

    outColor = texColor * fragColor * vec4(1.0, 1.0, 1.0, alpha) * vignette;
}
```

#### c) Farbverläufe

Da du bereits r, g, b Arrays für die Ecken hast, könntest du diese Farbwerte direkt in den VBO übernehmen:

```cpp
// In setupBuffers() oder bei einer VBO-Aktualisierung:
float vertices[] = {
    // Position        // Texturkoord.  // Farben
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f,    r[0], g[0], b[0], 1.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 0.0f,    r[1], g[1], b[1], 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 1.0f,    r[2], g[2], b[2], 1.0f,
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,    r[3], g[3], b[3], 1.0f
};
```

## 5. Weitere Schritte zur Modernisierung

Nach dem BackgroundManager könntest du schrittweise weitere Komponenten modernisieren:

1. **Andere einfache Sprites/Objekte**: z.B. UI-Elemente

2. **ParticleSystem**: Ideal für GPU-basiertes Instancing -> EffectManager

3. **Hauptspielelemente**: Bälle, Ziegel etc.

Jeder Schritt sollte den gleichen Fallback-Mechanismus enthalten, damit das Spiel während der Migration immer
funktionsfähig bleibt.

## 6. Integration mit dem EventManager

Später könntest du den modernisierten BackgroundManager sogar mit dem EventManager koppeln, wie in deinen vorherigen
Diskussionen erwähnt:

```cpp
// In BackgroundManager.h:
void onEvent(const GameEvent& event);

// In BackgroundManager.cpp:
void BackgroundManager::onEvent(const GameEvent& event) {
    switch (event.type) {
        case EVENT_THEME_CHANGED:
            // Hintergrund-Theme anpassen
            break;
        case EVENT_LEVEL_STARTED:
            // Level-spezifische Hintergründe laden
            break;
    }
}
```

Dieser modernisierte BackgroundManager ist ein guter erster Schritt in Richtung einer vollständigen Modernisierung
deines Spiels mit Shadern, unter Beibehaltung der Kompatibilität mit bestehenden Teilen des Codes.

## Hintergrund-Kacheln für 4:3 Format

Für ein Kachel-System (Tiling) in einem 4:3 Hintergrund solltest du ein paar Faktoren berücksichtigen:

### Optimale Kachelgröße

Bei einem 4:3 Format lassen sich Kacheln besonders gut verwenden, wenn sie ein Teiler der Gesamtauflösung sind. Für
typische Auflösungen wie:

- 640×480 (4:3)
- 800×600 (4:3)
- 1024×768 (4:3)

Sinnvolle Kachelgrößen wären:

- 32×32 Pixel (für kleine Texturen)
- 64×64 Pixel (guter Kompromiss)
- 128×96 Pixel (für detailliertere Kacheln)

### Implementierungsvorschlag

Hier ist eine Erweiterung deines `BackgroundManager`, um Kachel-Hintergründe zu unterstützen:

```cpp
struct TileInfo {
    float scaleX{1.0f};
    float scaleY{1.0f};
    bool repeat{true};
    float scrollSpeed{0.0f};  // für Parallax-Effekte
};

class BackgroundManager {
    // Bestehende Attribute
    GLuint backgroundDisplayList;
    GLfloat r[4], g[4], b[4], a;
    const TextureManager* textureMgr;
    SpriteSheetAnimation tex;
    
    // Neue Attribute
    bool isTiled{false};
    TileInfo tileInfo;
    float scrollOffset{0.0f};
    
    // Methoden
    void drawQuad() const;
    void drawTiledQuad() const;

public:
    explicit BackgroundManager(const TextureManager &texMgr);
    ~BackgroundManager();

    bool updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName);
    bool loadTiledBackground(const std::filesystem::path &imagePath, const std::filesystem::path &configPath);
    void update(float deltaTime);
    void draw() const;
};
```

Die Implementierung der `drawTiledQuad` Methode könnte so aussehen:

```cpp
void BackgroundManager::drawTiledQuad() const {
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
    
    // Kachel-Wiederholung aktivieren
    if (tileInfo.repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    float texCoordX = tileInfo.scaleX * 4.0f / 3.0f; // Anpassung an das 4:3 Format
    float texCoordY = tileInfo.scaleY;
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f + scrollOffset, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(texCoordX + scrollOffset, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(texCoordX + scrollOffset, texCoordY);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f + scrollOffset, texCoordY);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
```

## JSON-Konfigurationsdatei

Du könntest für jede Kachel eine einfache JSON-Datei mit dem gleichen Namen wie das Bild verwenden:

```json
{
  "scaleX": 4.0,
  "scaleY": 3.0,
  "repeat": true,
  "scrollSpeed": 0.05
}
```

Mit dieser Implementierung würde eine 64×64 Kachel mit den oben genannten Einstellungen den 4:3 Bildschirm viermal
horizontal und dreimal vertikal wiederholt füllen.

## Animierte Hintergründe mit `scrollSpeed`

Ja, genau! Der `scrollSpeed`-Wert in der JSON-Konfiguration ist für animierte Hintergründe gedacht.

Mit diesem Parameter kannst du einen Parallax-Effekt (sanftes Scrollen) für deine gekachelten Hintergründe erstellen:

## Funktionsweise:

- `scrollSpeed: 0.05` bedeutet, dass sich die Textur-Koordinaten mit jedem Frame um 0.05 Einheiten verschieben
- Positive Werte bewegen die Textur nach rechts, negative nach links
- Der Wert beeinflusst, wie schnell sich der Hintergrund bewegt

Die Bewegung wird in der `update`-Methode des `BackgroundManager` implementiert:

```cpp
void BackgroundManager::update(float deltaTime) {
    if (isTiled && tileInfo.scrollSpeed != 0.0f) {
        // Scrolloffset aktualisieren
        scrollOffset += tileInfo.scrollSpeed * deltaTime;
        
        // Zurücksetzen, wenn der Offset zu groß wird (endlose Schleife)
        if (scrollOffset > 1.0f) {
            scrollOffset -= 1.0f;
        }
        else if (scrollOffset < -1.0f) {
            scrollOffset += 1.0f;
        }
    }
}
```

Diese Technik wird häufig verwendet für:

- Wolkenbewegungen
- Sternenfelder im Weltraum
- Wasseroberflächen
- Jede Art von subtiler Hintergrundbewegung, die Tiefe erzeugt

Du kannst auch verschiedene Ebenen mit unterschiedlichen Geschwindigkeiten haben, um einen echten Parallax-Effekt zu
erzeugen.

## Shader-Integration (Shadertoy-Shadern)

{
"background": {
"type": "shader",
"vertex": "shaders/basic.vert",
"fragment": "shaders/stars.frag"
}
}
