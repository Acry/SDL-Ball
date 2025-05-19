# GL 1.1 bis 3.1

Zwischen OpenGL 1.1 und 3.1 gab es viele Verbesserungen, aber die meisten großen Änderungen betreffen Shader und moderne Pipeline. Ohne Shader kannst du vor allem folgende Verbesserungen nutzen:

Vertex Buffer Objects (VBOs):
Statt glBegin/glEnd kannst du Geometrie effizienter mit VBOs speichern und rendern (glGenBuffers, glBindBuffer, glBufferData, glVertexPointer).

Vertex Array Objects (VAOs):
VAOs speichern den Zustand von Vertex-Attributen und VBO-Bindings, was das Umschalten zwischen verschiedenen Geometrien vereinfacht.

glMapBuffer / glUnmapBuffer:
Direkter Zugriff auf Buffer-Daten im Speicher, z.B. für dynamische Geometrie.

Mehr Texturformate und größere Texturen:
Unterstützung für Non-Power-of-Two-Texturen, mehr interne Formate, bessere Kompression.

Multitexturing:
Mehrere Texturen gleichzeitig in der Fixed-Function-Pipeline (glActiveTexture, glMultiTexCoord*).

Frame Buffer Objects (FBOs):
Offscreen-Rendering, z.B. für Screenshots, Postprocessing, Shadowmaps.

Blending, Stencil, Depth Verbesserungen:
Mehr Kontrolle und Performance bei Alpha-Blending, Stencil- und Depth-Tests.

glDrawRangeElements, glDrawArrays:
Effizientere Zeichenbefehle für große Geometrien.

Wichtig:
Ab OpenGL 3.1 Core-Profil ist die Fixed-Function-Pipeline (glBegin, glVertex, glMatrixMode, glOrtho, ...) offiziell entfernt. Im Compatibility-Profil (wie bei dir) sind diese Funktionen aber weiterhin verfügbar.

Du kannst also VBOs, VAOs, FBOs, Multitexturing und moderne Texturformate nutzen, ohne auf Shader umzusteigen.
