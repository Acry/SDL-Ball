# Hintergrund-Kacheln für 4:3 Format

Für ein Kachel-System (Tiling) in einem 4:3 Hintergrund solltest du ein paar Faktoren berücksichtigen:

## Optimale Kachelgröße

Bei einem 4:3 Format lassen sich Kacheln besonders gut verwenden, wenn sie ein Teiler der Gesamtauflösung sind. Für typische Auflösungen wie:

- 640×480 (4:3)
- 800×600 (4:3)
- 1024×768 (4:3)

Sinnvolle Kachelgrößen wären:

- 32×32 Pixel (für kleine Texturen)
- 64×64 Pixel (guter Kompromiss)
- 128×96 Pixel (für detailliertere Kacheln)

## Implementierungsvorschlag

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

Mit dieser Implementierung würde eine 64×64 Kachel mit den oben genannten Einstellungen den 4:3 Bildschirm viermal horizontal und dreimal vertikal wiederholt füllen.

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

Du kannst auch verschiedene Ebenen mit unterschiedlichen Geschwindigkeiten haben, um einen echten Parallax-Effekt zu erzeugen.
  
## Shader-Integration (Shadertoy-Shadern)

{
  "background": {
    "type": "shader",
    "vertex": "shaders/basic.vert",
    "fragment": "shaders/stars.frag"
  }
}