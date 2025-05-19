# Rainbow-Tracer mit Shader-basiertem EffectManager

Ein moderner Ansatz für den Tracer wäre definitiv die Implementierung eines shader-basierten Systems, das besonders für einen Rainbow-Tracer mit dynamischen Farbverläufen geeignet ist.

## Fragment-Shader für den Rainbow-Tracer

Hier ist ein Fragment-Shader, der einen pulsierenden, regenbogenfarbenen Tracer erzeugt:

```glsl
// Fragment-Shader für Rainbow-Tracer
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform float uTime;       // Laufende Zeit für Animation
uniform float uLifetime;   // Gesamtlebensdauer des Partikels
uniform float uSize;       // Partikelgröße
uniform vec4 uColor;       // Basis-Farbmodifikator

// HSV zu RGB Konvertierung
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Berechne normalisierte Position vom Zentrum
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoord, center);
    
    // Zeitfaktor (1.0 am Anfang, 0.0 am Ende)
    float timeRatio = 1.0 - clamp(uTime / uLifetime, 0.0, 1.0);
    
    // Regenbogenfarbe basierend auf Zeit und Position
    float hue = mod(uTime * 0.3 + dist * 2.0, 1.0);
    float sat = 0.8;
    float val = 1.0;
    vec3 rainbowColor = hsv2rgb(vec3(hue, sat, val));
    
    // Mit Basis-Farbe modulieren
    rainbowColor *= uColor.rgb;
    
    // Glow-Effekt mit weichem Rand
    float glow = smoothstep(0.5, 0.1, dist);
    
    // Pulsieren
     alpha * uColor.a);
}


##float pulse = 0.8 + 0.2 * sin(uTime * 10.0);

    // Alpha-Transparenz kombiniert Zeit, Glow und Pulsieren
    float alpha = glow * timeRatio * timeRatio * pulse;
    
    // Endgültige Farbe
    FragColor = vec4(rainbowColor, Integration in ModernEffectManager
```

Um den bisherigen `EffectManager` zu modernisieren und auf Shader umzustellen, könnte folgende Implementierung verwendet werden:

```cpp
class ModernEffectManager : public IEventListener {
private:
    // ShaderManager für Shader-Verwaltung
    class ShaderManager {
    public:
        GLuint loadShaderPair(const std::string& vertexSource, const std::string& fragmentSource);
        void setUniform(GLuint program, const std::string& name, float value);
        void setUniform(GLuint program, const std::string& name, const glm::vec4& value);
        // weitere Hilfsfunktionen...
    };

    struct ParticleEffect {
        int id;
        bool active;
        int type;
        GLuint shaderProgram;
        glm::vec3 position;
        glm::vec4 color;
        float size;
        float lifetime;
        float startTime;
        float currentTime;
    };

    // Manager und Ressourcen
    EventManager& eventMgr;
    ShaderManager shaderMgr;
    GLuint particleVAO;
    GLuint particleVBO;
    
    // Shader-Programme
    GLuint tracerShader;
    GLuint explosionShader;
    GLuint sparkShader;
    
    // Aktive Effekte
    std::vector<ParticleEffect> activeEffects;
    int nextEffectId = 0;

public:
    ModernEffectManager(EventManager& eventManager) : eventMgr(eventManager) {
        initShaders();
        setupGeometry();
        
        // Als Listener registrieren
        eventMgr.registerListener(this, EVENT_BALL_MOVED);
        eventMgr.registerListener(this, EVENT_BRICK_DESTROYED);
        // weitere Events...
    }
    
    ~ModernEffectManager() {
        eventMgr.unregisterListener(this);
        // Shader und Ressourcen freigeben...
    }

    // Event-Handler
    void onEvent(const GameEvent& event) override {
        switch(event.type) {
            case EVENT_BALL_MOVED:
                if (event.data.ballData.isActive && event.data.ballData.hasEyeCandy &&
                    glm::distance(event.data.ballData.currentPos, event.data.ballData.lastPos) > 0.01f) {
                    spawnTracer(glm::vec3(event.data.ballData.currentPos, 0.0f));
                }
                break;
                
            case EVENT_BRICK_DESTROYED:
                spawnExplosion(glm::vec3(event.data.brickData.position, 0.0f));
                break;
                
            // Weitere Events...
        }
    }
    
    // Tracer-spezifische Methode
    int spawnTracer(const glm::vec3& position) {
        ParticleEffect effect;
        effect.id = nextEffectId++;
        effect.active = true;
        effect.type = EFFECT_TRACER;
        effect.shaderProgram = tracerShader;
        effect.position = position;
        effect.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);  // Default: weiß
        effect.size = 0.15f;
        effect.lifetime = 0.8f;  // 800ms
        effect.startTime = getCurrentTime();
        effect.currentTime = 0.0f;
        
        activeEffects.push_back(effect);
        return effect.id;
    }
    
    // Weitere Effekt-Methoden...
    int spawnExplosion(const glm::vec3& position) {
        // Ähnlich wie spawnTracer, aber mit anderen Parametern
    }
    
    // Aktualisierung und Rendering
    void update(float deltaTime) {
        // Aktualisiere alle aktiven Effekte
        for (auto& effect : activeEffects) {
            if (!effect.active) continue;
            
            effect.currentTime += deltaTime;
            if (effect.currentTime >= effect.lifetime) {
                effect.active = false;
            }
        }
        
        // Entferne inaktive Effekte
        activeEffects.erase(
            std::remove_if(activeEffects.begin(), activeEffects.end(),
                [](const ParticleEffect& e) { return !e.active; }),
            activeEffects.end()
        );
    }
    
    void render() {
        glBindVertexArray(particleVAO);
        
        // Sortiere Effekte nach Shader für Batch-Rendering
        std::sort(activeEffects.begin(), activeEffects.end(),
                 [](const ParticleEffect& a, const ParticleEffect& b) {
                     return a.shaderProgram < b.shaderProgram;
                 });
        
        GLuint currentShader = 0;
        
        // Render alle aktiven Effekte
        for (const auto& effect : activeEffects) {
            if (!effect.active) continue;
            
            // Shader wechseln wenn nötig
            if (currentShader != effect.shaderProgram) {
                currentShader = effect.shaderProgram;
                glUseProgram(currentShader);
            }
            
            // Uniforms setzen
            shaderMgr.setUniform(currentShader, "uTime", effect.currentTime);
            shaderMgr.setUniform(currentShader, "uLifetime", effect.lifetime);
            shaderMgr.setUniform(currentShader, "uSize", effect.size);
            shaderMgr.setUniform(currentShader, "uColor", effect.color);
            
            // Position setzen
            glm::mat4 model = glm::translate(glm::mat4(1.0f), effect.position);
            shaderMgr.setUniform(currentShader, "uModel", model);
            
            // Zeichnen
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Quad für Partikel
        }
        
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    void kill(int effectId) {
        for (auto& effect : activeEffects) {
            if (effect.id == effectId) {
                effect.active = false;
                return;
            }
        }
    }
    
    // Hilfsmethoden
    void setupGeometry() {
        // Einfaches Quad für Partikel erstellen
        float quadVertices[] = {
            // Positionen    // TexCoords
            -0.5f, -0.5f,    0.0f, 0.0f,
             0.5f, -0.5f,    1.0f, 0.0f,
             0.5f,  0.5f,    1.0f, 1.0f,
            -0.5f,  0.5f,    0.0f, 1.0f
        };
        
        glGenVertexArrays(1, &particleVAO);
        glGenBuffers(1, &particleVBO);
        
        glBindVertexArray(particleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        
        // Position-Attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // TexCoord-Attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
    }
    
    void initShaders() {
        // Vertex-Shader für alle Partikel
        std::string vertexShader = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec2 aTexCoord;
            
            uniform mat4 uProjection;
            uniform mat4 uModel;
            uniform float uSize;
            
            out vec2 TexCoord;
            
            void main() {
                gl_Position = uProjection * uModel * vec4(aPos * uSize, 0.0, 1.0);
                TexCoord = aTexCoord;
            }
        )";
        
        // Fragment-Shader für Tracer-Partikel
        std::string tracerFragmentShader = R"(
            #version 330 core
            
            in vec2 TexCoord;
            out vec4 FragColor;
            
            uniform float uTime;
            uniform float uLifetime;
            uniform float uSize;
            uniform vec4 uColor;
            
            // HSV zu RGB Konvertierung
            vec3 hsv2rgb(vec3 c) {
                vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }
            
            void main() {
                // Berechne normalisierte Position vom Zentrum
                vec2 center = vec2(0.5, 0.5);
                float dist = distance(TexCoord, center);
                
                // Zeitfaktor (1.0 am Anfang, 0.0 am Ende)
                float timeRatio = 1.0 - clamp(uTime / uLifetime, 0.0, 1.0);
                
                // Regenbogenfarbe basierend auf Zeit und Position
                float hue = mod(uTime * 0.3 + dist * 2.0, 1.0);
                float sat = 0.8;
                float val = 1.0;
                vec3 rainbowColor = hsv2rgb(vec3(hue, sat, val));
                
                // Mit Basis-Farbe modulieren
                rainbowColor *= uColor.rgb;
                
                // Glow-Effekt mit weichem Rand
                float glow = smoothstep(0.5, 0.1, dist);
                
                // Pulsieren
                float pulse = 0.8 + 0.2 * sin(uTime * 10.0);
                
                // Alpha-Transparenz kombiniert Zeit, Glow und Pulsieren
                float alpha = glow * timeRatio * timeRatio * pulse;
                
                // Endgültige Farbe
                FragColor = vec4(rainbowColor, alpha * uColor.a);
            }
        )";
        
        // Weitere Fragment-Shader für andere Effekttypen...
        
        // Shader kompilieren und verlinken
        tracerShader = shaderMgr.loadShaderPair(vertexShader, tracerFragmentShader);
        // explosionShader = ...
    }
};
```

## Integration mit dem EventManager

Für eine vollständige Integration würden wir auch den EventManager benötigen:

```cpp
// Minimal notwendige Event-Definitionen
enum EventType {
    EVENT_BALL_MOVED,
    EVENT_BRICK_DESTROYED,
    // weitere Events...
    EVENT_ALL  // Spezialwert für Abmeldung von allen Events
};

struct BallMoveData {
    glm::vec2 currentPos;
    glm::vec2 lastPos;
    bool isActive;
    bool hasEyeCandy;
};

struct BrickData {
    glm::vec2 position;
    int type;
    int points;
};

// Union für verschiedene Event-Datentypen
union EventData {
    BallMoveData ballData;
    BrickData brickData;
    // weitere Datentypen...
};

struct GameEvent {
    EventType type;
    EventData data;
};

// Interface für Event-Listener
class IEventListener {
public:
    virtual void onEvent(const GameEvent& event) = 0;
    virtual ~IEventListener() = default;
};
```

## Theming mit JSON-Konfiguration

Ein Vorteil dieses Ansatzes ist die einfache Konfiguration durch Theme-Dateien:

```json
{
  "effects": {
    "tracer": {
      "shader": "rainbow_tracer",
      "size": 0.15,
      "lifetime": 0.8,
      "baseColor": [1.0, 1.0, 1.0, 0.8],
      "pulseSpeed": 10.0,
      "rainbowSpeed": 0.3
    },
    "explosion": {
      "shader": "particle_explosion",
      "size": 0.3,
      "lifetime": 1.2,
      "baseColor": [1.0, 0.5, 0.1, 1.0],
      "particleCount": 50,
      "expansionRate": 2.0
    }
  }
}
```

## Vorteile dieses Ansatzes

1. **GPU-Beschleunigung**: Die Shader-basierte Implementierung nutzt die Grafikhardware optimal
2. **Visuelle Qualität**: Weiche Übergänge und dynamische Farbverläufe sind einfach umzusetzen
3. **Skalierbarkeit**: Tausende von Partikeln sind ohne Performanceeinbußen möglich
4. **Modularität**: Klare Trennung zwischen Rendering (Shader) und Logik (Manager)
5. **Konfigurierbarkeit**: Einfache Anpassung durch Theme-Dateien ohne Code-Änderungen

## Schrittweise Implementierung

Um die bestehende EffectManager-Struktur zu modernisieren:

1. **Shader-Infrastruktur implementieren**:
    - ShaderManager zum Laden und Verwalten von Shadern
    - VAO/VBO für Partikelgeometrie erstellen

2. **Events definieren**:
    - EVENT_BALL_MOVED für Tracer
    - EVENT_BRICK_DESTROYED für Explosionen
    - Weitere Events nach Bedarf

3. **Bestehenden EffectManager erweitern**:
    - Shader-Unterstützung hinzufügen, altes System beibehalten
    - Allmählich Effekte auf Shader umstellen

4. **Theme-System implementieren**:
    - JSON-Parser für Effekt-Konfigurationen
    - Dynamisches Laden von Shader-Parametern

Diese Modernisierung würde nicht nur einen beeindruckenden Rainbow-Tracer ermöglichen, sondern auch das gesamte visuelle Erscheinungsbild des Spiels verbessern und gleichzeitig die Performance steigern.