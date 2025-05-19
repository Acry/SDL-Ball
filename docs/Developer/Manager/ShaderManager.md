# ShaderManager-Implementierung

Ein `ShaderManager` ist der ideale erste Schritt für die Modernisierung der OpenGL-Codebasis. Er bildet die Grundlage für alle shader-basierten Effekte und ermöglicht eine zentrale Verwaltung der Shader-Ressourcen.

## Grundstruktur des ShaderManagers

Hier ein Entwurf für einen flexiblen ShaderManager:

```cpp
// ShaderManager.h
#pragma once

#include <epoxy/gl.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <vector>
#include <memory>

class ShaderManager {
public:
    struct ShaderProgram {
        GLuint id;
        std::string name;
        
        // Standardkonstruktor
        ShaderProgram() : id(0), name("") {}
        
        // Hauptkonstruktor
        ShaderProgram(GLuint id, const std::string& name) : id(id), name(name) {}
        
        // Hilfsfunktionen
        void use() const { glUseProgram(id); }
        static void unuse() { glUseProgram(0); }
    };

private:
    // Gespeicherte Programme nach Namen
    std::unordered_map<std::string, ShaderProgram> shaderPrograms;
    
    // Aktuell verwendetes Programm
    const ShaderProgram* currentProgram = nullptr;
    
    // Fehlermeldungen
    std::string lastError;
    
    // Interne Hilfsfunktionen
    GLuint compileShader(GLenum type, const std::string& source);
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors(GLuint program);

public:
    ShaderManager();
    ~ShaderManager();
    
    // Shader erstellen und kompilieren
    bool createFromStrings(const std::string& name, 
                          const std::string& vertexCode,
                          const std::string& fragmentCode);
    
    // Shader aus Dateien laden
    bool createFromFiles(const std::string& name,
                        const std::filesystem::path& vertexPath,
                        const std::filesystem::path& fragmentPath);
    
    // Standardshader für 2D-Grafiken
    bool createDefaultShaders();
    
    // Shader aktivieren/deaktivieren
    bool useShader(const std::string& name);
    void unuseShader();
    
    // Shader löschen
    void deleteShader(const std::string& name);
    
    // Shader abrufen
    const ShaderProgram* getShader(const std::string& name) const;
    const ShaderProgram* getCurrentShader() const;
    
    // Hilfsfunktionen für Uniformvariablen
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);
    void setBool(const std::string& name, bool value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    
    // Fehlermeldungen
    const std::string& getLastError() const;
    
    // Ressourcenbereinigung
    void cleanup();
};
```

## Implementierung

```cpp
// ShaderManager.cpp
#include "ShaderManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderManager::ShaderManager() {
    // Überprüfen, ob Shader unterstützt werden
    if (!glewIsSupported("GL_VERSION_3_1")) {
        lastError = "Shader werden nicht unterstützt (OpenGL 3.1 erforderlich)";
        std::cerr << lastError << std::endl;
    }
}

ShaderManager::~ShaderManager() {
    cleanup();
}

bool ShaderManager::createFromStrings(const std::string& name, 
                                     const std::string& vertexCode,
                                     const std::string& fragmentCode) {
    // Bereits existierende Shader überschreiben
    if (shaderPrograms.find(name) != shaderPrograms.end()) {
        glDeleteProgram(shaderPrograms[name].id);
    }
    
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    if (vertexShader == 0) return false;
    
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Shader-Programm erstellen und verknüpfen
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    // Fehlerprüfung beim Linken
    if (!checkLinkErrors(programID)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        return false;
    }
    
    // Shader-Objekte nach dem Linking freigeben
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Shader-Programm speichern
    shaderPrograms[name] = ShaderProgram(programID, name);
    return true;
}

bool ShaderManager::createFromFiles(const std::string& name,
                                   const std::filesystem::path& vertexPath,
                                   const std::filesystem::path& fragmentPath) {
    // Dateien einlesen
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    // Exceptions für Fehlerbehandlung aktivieren
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Dateien öffnen
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        
        // Inhalte in Stringstreams einlesen
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        // Dateien schließen
        vShaderFile.close();
        fShaderFile.close();
        
        // Streams in Strings umwandeln
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        lastError = "Fehler beim Lesen der Shader-Dateien: " + std::string(e.what());
        return false;
    }
    
    // Strings an createFromStrings übergeben
    return createFromStrings(name, vertexCode, fragmentCode);
}

bool ShaderManager::createDefaultShaders() {
    // Einfacher Vertex-Shader für 2D-Texturen
    const std::string vertexShaderCode = R"(
        #version 150
        in vec3 aPosition;
        in vec2 aTexCoord;
        in vec4 aColor;
        
        out vec2 TexCoord;
        out vec4 Color;
        
        uniform mat4 transform;
        
        void main() {
            gl_Position = transform * vec4(aPosition, 1.0);
            TexCoord = aTexCoord;
            Color = aColor;
        }
    )";
    
    // Einfacher Fragment-Shader für 2D-Texturen
    const std::string fragmentShaderCode = R"(
        #version 150
        in vec2 TexCoord;
        in vec4 Color;
        
        out vec4 FragColor;
        
        uniform sampler2D texSampler;
        uniform float alpha;
        
        void main() {
            vec4 texColor = texture(texSampler, TexCoord);
            FragColor = texColor * Color * vec4(1.0, 1.0, 1.0, alpha);
        }
    )";
    
    return createFromStrings("default", vertexShaderCode, fragmentShaderCode);
}

GLuint ShaderManager::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* sourceCode = source.c_str();
    glShaderSource(shader, 1, &sourceCode, nullptr);
    glCompileShader(shader);
    
    // Fehlerprüfung bei der Kompilierung
    if (!checkCompileErrors(shader, (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT"))) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool ShaderManager::checkCompileErrors(GLuint shader, const std::string& type) {
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        lastError = "Fehler bei der Kompilierung des " + type + " Shaders: " + std::string(infoLog);
        std::cerr << lastError << std::endl;
        return false;
    }
    return true;
}

bool ShaderManager::checkLinkErrors(GLuint program) {
    int success;
    char infoLog[1024];
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        lastError = "Fehler beim Linken des Shader-Programms: " + std::string(infoLog);
        std::cerr << lastError << std::endl;
        return false;
    }
    return true;
}

bool ShaderManager::useShader(const std::string& name) {
    auto it = shaderPrograms.find(name);
    if (it == shaderPrograms.end()) {
        lastError = "Shader-Programm '" + name + "' nicht gefunden";
        return false;
    }
    
    glUseProgram(it->second.id);
    currentProgram = &(it->second);
    return true;
}

void ShaderManager::unuseShader() {
    glUseProgram(0);
    currentProgram = nullptr;
}

void ShaderManager::deleteShader(const std::string& name) {
    auto it = shaderPrograms.find(name);
    if (it != shaderPrograms.end()) {
        if (currentProgram == &(it->second)) {
            unuseShader();
        }
        glDeleteProgram(it->second.id);
        shaderPrograms.erase(it);
    }
}

const ShaderManager::ShaderProgram* ShaderManager::getShader(const std::string& name) const {
    auto it = shaderPrograms.find(name);
    if (it != shaderPrograms.end()) {
        return &(it->second);
    }
    return nullptr;
}

const ShaderManager::ShaderProgram* ShaderManager::getCurrentShader() const {
    return currentProgram;
}

void ShaderManager::setFloat(const std::string& name, float value) {
    if (currentProgram) {
        glUniform1f(glGetUniformLocation(currentProgram->id, name.c_str()), value);
    }
}

void ShaderManager::setInt(const std::string& name, int value) {
    if (currentProgram) {
        glUniform1i(glGetUniformLocation(currentProgram->id, name.c_str()), value);
    }
}

void ShaderManager::setBool(const std::string& name, bool value) {
    if (currentProgram) {
        glUniform1i(glGetUniformLocation(currentProgram->id, name.c_str()), (int)value);
    }
}

void ShaderManager::setVec2(const std::string& name, const glm::vec2& value) {
    if (currentProgram) {
        glUniform2fv(glGetUniformLocation(currentProgram->id, name.c_str()), 1, glm::value_ptr(value));
    }
}

void ShaderManager::setVec3(const std::string& name, const glm::vec3& value) {
    if (currentProgram) {
        glUniform3fv(glGetUniformLocation(currentProgram->id, name.c_str()), 1, glm::value_ptr(value));
    }
}

void ShaderManager::setVec4(const std::string& name, const glm::vec4& value) {
    if (currentProgram) {
        glUniform4fv(glGetUniformLocation(currentProgram->id, name.c_str()), 1, glm::value_ptr(value));
    }
}

void ShaderManager::setMat3(const std::string& name, const glm::mat3& value) {
    if (currentProgram) {
        glUniformMatrix3fv(glGetUniformLocation(currentProgram->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderManager::setMat4(const std::string& name, const glm::mat4& value) {
    if (currentProgram) {
        glUniformMatrix4fv(glGetUniformLocation(currentProgram->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}

const std::string& ShaderManager::getLastError() const {
    return lastError;
}

void ShaderManager::cleanup() {
    for (auto& pair : shaderPrograms) {
        glDeleteProgram(pair.second.id);
    }
    shaderPrograms.clear();
    currentProgram = nullptr;
}
```

## Integration mit dem BackgroundManager

Nachdem wir den ShaderManager implementiert haben, können wir den BackgroundManager wie folgt anpassen:

```cpp
// BackgroundManager.h (mit Shader-Unterstützung)
#pragma once
#include <filesystem>

#include "SpriteSheetAnimation.h"
#include "ShaderManager.h"

class TextureManager;
class BackgroundManager {
    // Legacy-Ressourcen
    GLuint backgroundDisplayList;
    
    // Moderne OpenGL-Ressourcen
    GLuint VAO, VBO, EBO;
    bool useModernRendering;
    
    // Farbwerte
    GLfloat r[4], g[4], b[4], a;
    
    // Manager-Referenzen
    const TextureManager* textureMgr;
    ShaderManager* shaderMgr;
    
    // Texturen
    SpriteSheetAnimation tex;

    // Interne Hilfsmethoden
    void drawQuad() const;
    void setupModernBuffers();

public:
    BackgroundManager(const TextureManager &texMgr, ShaderManager &shaderMgr);
    ~BackgroundManager();

    bool updateBgIfNeeded(const Uint32 level, const std::filesystem::path &pathName);
    void draw() const;
};
```

```cpp
// BackgroundManager.cpp (mit Shader-Unterstützung)
#include "BackgroundManager.h"
#include "TextureManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BackgroundManager::BackgroundManager(const TextureManager &texMgr, ShaderManager &shaderMgr) 
    : backgroundDisplayList(0), VAO(0), VBO(0), EBO(0), useModernRendering(false),
      textureMgr(&texMgr), shaderMgr(&shaderMgr) {
    
    // Überprüfen, ob moderne Rendering-Features unterstützt werden
    useModernRendering = glewIsSupported("GL_VERSION_3_1") != 0;
    
    // Farben initialisieren
    for (int i = 0; i < 4; i++) {
        r[i] = 0.9f;
        g[i] = 0.9f;
        b[i] = 0.9f;
    }
    a = 0.95f;
    
    // Default-Shader erstellen, falls benötigt
    if (useModernRendering && !shaderMgr.getShader("background")) {
        // Spezifischer Hintergrund-Shader
        const std::string vertexShaderCode = R"(
            #version 150
            in vec3 aPosition;
            in vec2 aTexCoord;
            in vec4 aColor;
            
            out vec2 TexCoord;
            out vec4 Color;
            
            void main() {
                gl_Position = vec4(aPosition, 1.0);
                TexCoord = aTexCoord;
                Color = aColor;
            }
        )";
        
        const std::string fragmentShaderCode = R"(
            #version 150
            in vec2 TexCoord;
            in vec4 Color;
            
            out vec4 FragColor;
            
            uniform sampler2D texSampler;
            uniform float alpha;
            
            void main() {
                vec4 texColor = texture(texSampler, TexCoord);
                FragColor = texColor * Color * vec4(1.0, 1.0, 1.0, alpha);
            }
        )";
        
        shaderMgr.createFromStrings("background", vertexShaderCode, fragmentShaderCode);
    }
}

void BackgroundManager::setupModernBuffers() {
    if (VAO != 0) {
        // Bereits eingerichtet
        return;
    }
    
    // Vertex-Daten mit Position, Texturkoordinaten und Farbe
    float vertices[] = {
        // Position (x,y,z)    // Texturkoordinaten  // Farben
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,           r[0], g[0], b[0], 1.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f,           r[1], g[1], b[1], 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f,           r[2], g[2], b[2], 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,           r[3], g[3], b[3], 1.0f
    };
    
    // Indices für Dreiecke
    unsigned int indices[] = {
        0, 1, 2,    // Erstes Dreieck
        0, 2, 3     // Zweites Dreieck
    };
    
    // VAO und VBOs erstellen
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // VBO für Vertices
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // EBO für Indices
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
    
    // VAO unbinden
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
        // Legacy-Display-Liste aktualisieren
        if (backgroundDisplayList != 0) {
            glDeleteLists(backgroundDisplayList, 1);
        }
        backgroundDisplayList = glGenLists(1);
        glNewList(backgroundDisplayList, GL_COMPILE);
        drawQuad();
        glEndList();
        
        // Moderne Buffer aktualisieren
        if (useModernRendering) {
            setupModernBuffers();
        }
    }
    
    return result;
}

void BackgroundManager::draw() const {
    if (useModernRendering && VAO != 0 && shaderMgr->getShader("background")) {
        // Moderner Rendering-Pfad mit Shadern
        shaderMgr->useShader("background");
        
        // Textur binden
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
        
        // Shader-Uniforms setzen
        shaderMgr->setInt("texSampler", 0);
        shaderMgr->setFloat("alpha", a);
        
        // Alpha-Blending aktivieren
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Zeichnen
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // Aufräumen
        shaderMgr->unuseShader();
        glDisable(GL_BLEND);
    } else if (backgroundDisplayList != 0) {
        // Legacy-Pfad mit Display-Listen
        glCallList(backgroundDisplayList);
    }
}

void BackgroundManager::drawQuad() const {
    // Legacy-Code für Display-Listen
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, tex.textureProperties.texture);
    glBegin(GL_QUADS);
    glColor4f(r[0], g[0], b[0], a);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glColor4f(r[1], g[1], b[1], a);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glColor4f(r[2], g[2], b[2], a);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glColor4f(r[3], g[3], b[3], a);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

BackgroundManager::~BackgroundManager() {
    // Legacy-Ressourcen freigeben
    if (backgroundDisplayList != 0) {
        glDeleteLists(backgroundDisplayList, 1);
    }
    
    // Moderne OpenGL-Ressourcen freigeben
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}
```

## Shader-Erweiterungen für den BackgroundManager

Hier sind ein paar fortgeschrittene Shader für den BackgroundManager, die du später einsetzen könntest:

### 1. Welleneffekt

```glsl
// Fragment-Shader mit Welleneffekt
#version 150
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform float alpha;
uniform float time;  // Zeit in Sekunden

void main() {
    // Wellenparameter
    float waveFrequency = 10.0;
    float waveAmplitude = 0.015;
    
    // Welleneffekt auf Texturkoordinaten
    vec2 distortedCoords = TexCoord;
    distortedCoords.x += sin(TexCoord.y * waveFrequency + time) * waveAmplitude;
    distortedCoords.y += cos(TexCoord.x * waveFrequency + time) * waveAmplitude;
    
    vec4 texColor = texture(texSampler, distortedCoords);
    FragColor = texColor * Color * vec4(1.0, 1.0, 1.0, alpha);
}
```

### 2. Vignette und Farbkorrektur

```glsl
// Fragment-Shader mit Vignette-Effekt
#version 150
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform float alpha;
uniform float vignetteStrength;  // Stärke des Vignette-Effekts
uniform vec3 colorTint;          // Farbton für den Hintergrund

void main() {
    // Textur abrufen
    vec4 texColor = texture(texSampler, TexCoord);
    
    // Vignette-Effekt
    vec2 position = TexCoord - 0.5;
    float len = length(position);
    float vignette = smoothstep(0.5, 0.35, len);
    
    // Farbtönung
    vec3 tintedColor = mix(texColor.rgb, colorTint, 0.2);
    
    // Ergebnis kombinieren
    vec3 finalColor = tintedColor * vignette;
    FragColor = vec4(finalColor, texColor.a * alpha) * Color;
}
```

### 3. Übergangseffekte

```glsl
// Fragment-Shader mit Übergangseffekt
#version 150
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform sampler2D nextTexSampler;  // Nächstes Bild für Überblendung
uniform float alpha;
uniform float transitionAmount;     // 0.0 bis 1.0 für den Übergang

void main() {
    // Beide Texturen abrufen
    vec4 texColor1 = texture(texSampler, TexCoord);
    vec4 texColor2 = texture(nextTexSampler, TexCoord);
    
    // Überblendung zwischen den Bildern
    vec4 texColor = mix(texColor1, texColor2, transitionAmount);
    
    // Endgültige Farbe
    FragColor = texColor * Color * vec4(1.0, 1.0, 1.0, alpha);
}
```

## Vorteile des ShaderManagers

1. **Zentrale Verwaltung**: Alle Shader werden an einem Ort verwaltet und können von verschiedenen Komponenten genutzt werden.

2. **Einfachere Integration**: Der Umstieg von Legacy-Rendering auf Shader-basiertes Rendering wird durch die Abstraktion vereinfacht.

3. **Fallback-Mechanismus**: Unterstützt sowohl moderne als auch Legacy-OpenGL-Systeme.

4. **Fehlerbehandlung**: Robuste Fehlerbehandlung für Shader-Kompilierung und -Linking.

5. **Erweiterbarkeit**: Einfaches Hinzufügen neuer Shader für spezielle Effekte und Themes.

6. **Ressourcenmanagement**: Automatische Bereinigung von Shader-Ressourcen verhindert Memory-Leaks.

## Nächste Schritte

1. **Implementierung des ShaderManagers** als eigenständige Komponente.

2. **Integration in den BackgroundManager** wie oben beschrieben.

3. **Effekt-Erweiterungen** durch spezialisierte Shader für bestimmte visuelle Effekte.

4. **Event-basierte Theme-Unterstützung**: Integration mit dem EventManager für dynamische Shader-Wechsel.

5. **Graduelle Umstellung** weiterer Komponenten wie dem EffectManager und dem Tracer auf shader-basiertes Rendering.

Mit diesem ShaderManager hast du eine solide Grundlage für die Modernisierung deines OpenGL-Codes, während du gleichzeitig die Kompatibilität mit dem bestehenden Legacy-Code beibehältst.