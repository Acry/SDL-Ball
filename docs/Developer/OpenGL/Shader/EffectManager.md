# Shader-basierter Ansatz für Tracer und EffectManager



Ein moderner Ansatz für visuelle Effekte wie den Tracer wäre, auf Shader zu setzen.
Gerade in einem 2D-Spiel können Fragment-Shader die Partikeldarstellung erheblich optimieren und flexibilisieren.

## Vorteile von Shadern für Partikeleffekte

1. **Performancegewinn**:
    - GPU-beschleunigte Berechnung statt CPU-lastiger Partikelaktualisierung
    - Möglichkeit, tausende Partikel gleichzeitig darzustellen
    - Geringer Speicherverbrauch durch prozedurales Rendering

2. **Visuelle Qualität**:
    - Weiche Übergänge und Glow-Effekte
    - Dynamische Farbverläufe
    - Physikalisch plausiblere Partikelverhalten

"RainbowTracer" könnte z.B. durch einen Shader realisiert werden, der die Farbe der Partikel basierend auf ihrer Position und Zeit interpoliert.

3. **Konfigurierbarkeit**:
    - Einfache Anpassung von Partikelverhalten durch Shader-Parameter
    - Theming durch Austausch von Shadern oder Parametersätzen
    - Unterstützung verschiedener Partikelarten mit einem einzigen Shader

## EffectManager mit Shader-Integration

Ein moderner EffectManager könnte so strukturiert werden:

```cpp
class ModernEffectManager : public IEventListener {
private:
    struct ShaderProgram {
        GLuint programId;
        GLuint vertexShader;
        GLuint fragmentShader;
        // Uniform-Locations
        GLint uTime;
        GLint uColor;
        GLint uSize;
        GLint uLifetime;
    };

    struct ParticleEffect {
        int id;                 // Effekt-ID
        bool active;            // Ist aktiv
        ShaderProgram* shader;  // Zugewiesener Shader
        glm::vec3 position;     // Effektposition
        float size;             // Größe/Skalierung
        glm::vec4 color;        // Basisfarbe
        float lifetime;         // Lebensdauer
        float startTime;        // Startzeitpunkt
        std::vector<float> customParams; // Benutzerdefinierte Parameter
    };

    std::unordered_map<std::string, ShaderProgram> shaders;
    std::vector<ParticleEffect> activeEffects;

    // VAO/VBO für Partikelrendering
    GLuint particleVAO;
    GLuint particleVBO;

    // Shader-Verwaltung
    void loadShaders();
    void compileShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    void setupParticleBuffers();

public:
    ModernEffectManager(EventManager& eventManager);
    ~ModernEffectManager();

    // EffectManager-API
    int spawn(EffectType type, const glm::vec3& position);
    void update(float deltaTime);
    void render();
    void kill(int effectId);
    
    // Parameter-Setting
    void setEffectColor(int effectId, const glm::vec4& color);
    void setEffectSize(int effectId, float size);
    void setEffectLifetime(int effectId, float lifetime);
    void setEffectParam(int effectId, const std::string& paramName, float value);

    // EventListener-Implementation
    void onEvent(const GameEvent& event) override;
};
```

## Fragment-Shader für den Tracer

Ein moderner Tracer könnte einen speziellen Fragment-Shader verwenden, der weiche, leuchtende Partikel erzeugt:

```glsl
// Fragment-Shader für Tracer-Partikel
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 uColor;       // Basisfarbe mit Alpha
uniform float uTime;       // Aktuelle Zeit seit Start des Effekts
uniform float uLifetime;   // Gesamtlebensdauer
uniform float uSize;       // Partikelgröße

void main() {
    // Berechne normalisierte Position vom Zentrum (0-1)
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoord, center);
    
    // Zeitfaktor (1.0 am Anfang, 0.0 am Ende)
    float timeRatio = 1.0 - clamp(uTime / uLifetime, 0.0, 1.0);
    
    // Weicher Kreis mit Glow-Effekt
    float alpha = smoothstep(0.5, 0.0, dist);
    
    // Farbverlauf basierend auf Lebenszeit
    vec3 colorMod = mix(uColor.rgb, vec3(1.0, 1.0, 1.0), dist * 0.5);
    
    // Abnehmende Intensität über Lebensdauer
    float intensity = timeRatio * timeRatio;
    
    // Endgültige Farbe
    FragColor = vec4(colorMod, alpha * intensity * uColor.a);
}
```

## Vertex-Shader für Partikelpositionierung

```glsl
// Vertex-Shader für Partikel
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform float uSize;

out vec2 TexCoord;

void main() {
    // Position unter Berücksichtigung von Größe und Transformation
    gl_Position = uProjection * uModel * vec4(aPos * uSize, 0.0, 1.0);
    TexCoord = aTexCoord;
}
```

## Integration in den EventManager

Der modernisierte EffectManager würde über den EventManager kommunizieren:

```cpp
void ModernEffectManager::onEvent(const GameEvent& event) {
    switch(event.type) {
        case EVENT_BALL_MOVED: {
            // Tracer-Effekt bei Ballbewegung
            if (event.data.ballData.isActive && event.data.ballData.hasEyeCandy) {
                // Nur Tracer erzeugen, wenn sich der Ball signifikant bewegt hat
                if (glm::distance(event.data.ballData.currentPos, event.data.ballData.lastPos) > 0.01f) {
                    // Tracer-Partikel erzeugen
                    spawn(EFFECT_TRACER, glm::vec3(event.data.ballData.currentPos, 0.0f));
                }
            }
            break;
        }
        case EVENT_BRICK_DESTROYED: {
            // Explosions-Effekt bei Ziegelzerstörung
            spawn(EFFECT_EXPLOSION, glm::vec3(event.data.brickData.position, 0.0f));
            break;
        }
        // Weitere Events...
    }
}
```

## Theming durch Shader-Konfiguration

Ein großer Vorteil des Shader-Ansatzes ist die Möglichkeit, Themes über Shader-Konfigurationen zu definieren:

```cpp
// Beispiel: Theme-Konfiguration im JSON-Format
// theme.json
{
  "effects": {
    "tracer": {
      "shader": "glow_particle",
      "baseColor": [0.8, 0.2, 1.0, 0.7],
      "glowIntensity": 1.5,
      "fadeSpeed": 0.8,
      "size": 0.15
    },
    "explosion": {
      "shader": "explosion_particle",
      "baseColor": [1.0, 0.6, 0.1, 0.9],
      "expansionRate": 2.0,
      "particleCount": 50
    }
  }
}
```

Der ThemeManager könnte diese Konfigurationen laden und an den EffectManager übergeben:

```cpp
void ThemeManager::applyEffectTheme(ModernEffectManager& effectMgr) {
    // Lade Shader-Konfiguration aus der Theme-JSON
    auto& effectsConfig = currentTheme["effects"];
    
    // Konfiguriere Tracer-Effekt
    auto& tracerConfig = effectsConfig["tracer"];
    effectMgr.configureEffect(
        EFFECT_TRACER, 
        tracerConfig["shader"].get<std::string>(),
        glm::vec4(
            tracerConfig["baseColor"][0],
            tracerConfig["baseColor"][1], 
            tracerConfig["baseColor"][2],
            tracerConfig["baseColor"][3]
        ),
        tracerConfig["glowIntensity"],
        tracerConfig["fadeSpeed"],
        tracerConfig["size"]
    );
    
    // Weitere Effekte konfigurieren...
}
```

## Implementierungsschritte

Für den Übergang zu einem shader-basierten EffectManager:

1. **Abstraktion schaffen**:
    - Interface für Effekte definieren, das sowohl alte als auch neue Implementierung unterstützt
    - `IEffect`-Interface mit `update()` und `render()` Methoden

2. **Shader-Infrastruktur aufbauen**:
    - Shader-Lader implementieren
    - Shader-Kompilierung und Linking
    - Uniform-Handling

3. **Partikel-Rendering optimieren**:
    - Instanced Rendering für Partikelgruppen
    - Vertex Buffer Objects für Partikelgeometrie
    - Texture Arrays für verschiedene Partikeltypen

4. **Theme-Integration**:
    - Shader-Parameter in Themes definieren
    - JSON-Schema für Effekte erstellen
    - Theme-Parser erweitern

5. **Schrittweise Migration**:
    - Zunächst Tracer auf Shader umstellen
    - Dann weitere Effekte konvertieren
    - Kompatibilität mit altem System bewahren

Dieser Ansatz kombiniert moderne GPU-Nutzung mit den Vorteilen des Event-basierten Designs und ermöglicht gleichzeitig ein hochgradig anpassbares Theming-System.

