# Notes on OpenGL

## Plan an Incremental Transition

To avoid breaking the codebase, adopt an incremental refactoring strategy using the OpenGL compatibility profile (as you noted, supported in 3.1 compatibility). This allows legacy display lists to coexist with modern OpenGL code during the transition. Here’s a step-by-step plan:
Step 1: Set Up a Modern OpenGL Context
Use Compatibility Profile Initially: Continue using a 3.1 (or higher) compatibility profile to ensure glNewList and other legacy functions work. For example, with SDL:

```c++

SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
```

Plan for Core Profile Later: Once display lists are fully replaced, you can switch to a core profile (e.g., 3.3 or 4.5) to remove legacy dependencies.

Abstract Rendering Code
Create a Rendering Abstraction Layer: Introduce an abstraction (e.g., a Renderer or RenderObject class) to encapsulate rendering logic. This allows you to switch between display lists and modern rendering without changing high-level code.
Example:

```c++
class RenderObject {
public:
    virtual void render() = 0;
    virtual ~RenderObject() {}
};

class DisplayListObject : public RenderObject {
    GLuint listID;
public:
    DisplayListObject(GL HansonGLDisplayList(GLuint listID) : listID(listID) {}
    void render() override { glCallList(listID); }
};
```

Gradually Add Modern Implementations: Extend the abstraction to support modern OpenGL:

```c++
class VBOObject : public RenderObject {
    GLuint vao, vbo;
    ShaderProgram* shader;
    int vertexCount;
public:
    VBOObject(const std::vector<float>& vertices, ShaderProgram* shader)
        : shader(shader), vertexCount(vertices.size() / 3) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
    void render() override {
        shader->use();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }
    ~VBOObject() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }
};
```

Introduce Shaders for Fixed-Function Replacement
Display lists often include fixed-function state (e.g., glMaterial, glLight). Replace these with shaders.

Minimal Shader Example (for vertex positions and basic color)

```glsl
// Vertex Shader
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 modelViewProj;
void main() {
    gl_Position = modelViewProj * vec4(aPos, 1.0);
}

// Fragment Shader
#version 330 core
out vec4 FragColor;
uniform vec4 color;
void main() {
    FragColor = color;
}
```

Manage Uniforms: Replace state like glColor or glTranslate with uniform variables

shader->setUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
shader->setUniform("modelViewProj", modelViewProjMatrix);

Convert Display Lists to VBOs

Extract Geometry: For each display list, identify the geometry (vertices, normals, etc.). This may require parsing or logging commands within glNewList/glEndList blocks.

Convert immediate mode to vertex arrays

// Legacy display list
glNewList(listID, GL_COMPILE);
glBegin(GL_TRIANGLES);
glVertex3f(0.0f, 0.0f, 0.0f);
glVertex3f(1.0f, 0.0f, 0.0f);
glVertex3f(0.0f, 1.0f, 0.0f);
glEnd();
glEndList();

// Modern equivalent
std::vector<float> vertices = {
0.0f, 0.0f, 0.0f,
1.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f
};
auto* vboObject = new VBOObject(vertices, shader);

Batch Conversion: Prioritize frequently used or performance-critical display lists. Convert them one module or scene at a time (e.g., UI, terrain, models).

Preserve Display List Fallback: Keep display lists functional in the compatibility profile. Use a flag or conditional to switch between old and new rendering:

if (useModernRendering) {
vboObject->render();
} else {
glCallList(listID);
}

Handle State Changes
Display lists often embed state changes (e.g., glPushMatrix, glColor). In modern OpenGL:
Replace matrix operations with GLM or similar libraries and pass matrices as uniforms.
Replace material/lighting with shader uniforms or textures.

Example:
```c++
// Legacy
glPushMatrix();
glTranslatef(1.0f, 0.0f, 0.0f);
glCallList(listID);
glPopMatrix();

// Modern
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
shader->setUniform("modelViewProj", projection * view * model);
vboObject->render();
```

// Legacy
glPushMatrix();
glTranslatef(1.0f, 0.0f, 0.0f);
glCallList(listID);
glPopMatrix();

// Modern
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
shader->setUniform("modelViewProj", projection * view * model);
vboObject->render();

Step 6: Test Incrementally
Unit Tests: Write tests for each converted module to ensure rendering output matches the original (e.g., compare screenshots or render to texture).
Visual Debugging: Use tools like RenderDoc or NVIDIA Nsight to inspect VBOs, VAOs, and shader outputs.
Fallback Mechanism: If a modern rendering path fails, revert to display lists for that object or scene.

Step 7: Optimize and Clean Up
Merge VBOs: Combine small VBOs into larger buffers to reduce draw calls.
Instancing: For repeated geometry, use instanced rendering instead of multiple display list calls.
Remove Legacy Code: Once all display lists are replaced and tested, remove glNewList/glCallList calls and switch to a core profile (e.g., 3.3 or 4.5).

3. Tools and Libraries to Assist
   GLM: For matrix and vector operations (replaces glTranslate, glRotate).
   GLEW or GLAD: For loading modern OpenGL functions (required beyond OpenGL 1.1 on Windows).
   Assimp: To import models if you need to convert display list geometry to standard formats.
   RenderDoc: For debugging VBO/VAO setups and shader issues.
   Shader Management: Use a simple shader class to manage compilation and uniforms.

4. Challenges and Mitigations
   Complex Display Lists: Some lists may mix geometry and state changes extensively. Log or parse commands to separate geometry (for VBOs) and state (for uniforms).
   Performance Regression: Modern OpenGL can be slower initially if draw calls aren’t optimized. Profile with tools like NVIDIA Nsight to identify bottlenecks.
   Codebase Size: In a huge codebase, prioritize high-impact areas (e.g., main scene rendering) and use automation (e.g., scripts to extract vertex data) to scale conversion.
   Team Coordination: If multiple developers are involved, document the new rendering path and provide training on shaders and VBOs.

5. Example Timeline for a Huge Codebase
   Month 1-2: Audit codebase, set up compatibility profile, and create rendering abstraction.
   Month 3-4: Convert critical display lists to VBOs/VAOs, introduce basic shaders.
   Month 5-6: Expand to secondary systems (e.g., UI, particles), test extensively.
   Month 7-8: Optimize performance, merge VBOs, and remove unused display lists.
   Month 9+: Switch to core profile, clean up legacy code.

6. Maintaining Compatibility
   Compatibility Profile as a Bridge: Use the compatibility profile (3.1 or higher) to ensure display lists work until fully replaced.
   Hybrid Rendering: Allow objects to use either display lists or VBOs during the transition, controlled by a configuration flag.
   Versioned Shaders: Support multiple shader versions (e.g., #version 120 for compatibility, #version 330 for core) if needed.

Summary
To transition from display lists to modern OpenGL in a huge codebase without breaking it:
Use the OpenGL 3.1 compatibility profile to keep display lists functional.
Abstract rendering to support both legacy and modern paths.
Gradually convert display lists to VBOs/VAOs, starting with high-impact areas.
Replace fixed-function state with shaders and uniforms.
Test rigorously with fallbacks to display lists.
Optimize and eventually switch to a core profile.
By leveraging the compatibility profile and an incremental approach, you can refactor safely, ensuring the codebase remains functional while modernizing its rendering pipeline. If you have specific details about your codebase (e.g., types of display lists, rendering patterns), I can tailor this further!

## Legacy OpenGL

### Display Lists again

Understand the Role of Display Lists in Your Codebase
Display lists in legacy OpenGL are used to:

- Store static geometry (vertices, normals, texture coordinates, etc.).
- Encapsulate state changes (e.g., glMatrixMode, glColor, glMaterial).
- Optimize rendering by reducing CPU-GPU communication for static content.

In modern OpenGL (3.0+ core profile), display lists are replaced by:

- VBOs: Store vertex data on the GPU.
- VAOs: Encapsulate vertex attribute configurations.
- Shaders: Replace fixed-function pipeline state (e.g., lighting, transformations).
- Uniforms/UBOs: Manage state changes like matrices or material properties.

Before refactoring, analyze your codebase to identify:
Where display lists are used (e.g., for geometry, UI, or complex scenes).
The types of commands in display lists (geometry, state changes, or mixed).
Dependencies on fixed-function pipeline features (e.g., glBegin, glLight).
