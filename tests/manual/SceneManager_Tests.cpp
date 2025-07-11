// SceneManager_Tests.cpp
#include <cstdlib>
#include <vector>
#include <memory>
#include <chrono>
#include "DisplayManager.hpp"
#include "EffectFactory.h"
#include "EventDispatcher.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

struct SceneDefinition {
    std::string textureName;
    int duration;
    int transitionType;
};

class SceneManagerTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<EffectFactory> effectFactory;

    SceneManagerTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: SceneManager Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        effectFactory = std::make_unique<EffectFactory>(textureManager.get());
    }
};

class SceneManagerTestHelper final : public TestHelper {
    SceneManagerTestContext &ctx;
    std::vector<SceneDefinition> scenes;
    int currentSceneIndex = 0;
    bool isPlaying = true;
    std::chrono::steady_clock::time_point sceneStartTime;
    std::chrono::steady_clock::time_point pauseTime;
    std::unique_ptr<TransitionEffect> currentTransition;
    int lastTextureId = -1;
    int currentTextureId = -1;

    const std::vector<std::string> transitionNames = {
        "None", "FadeIn", "FadeOut", "CrossFade", "SlideIn", "SlideOut", "WipeIn", "WipeOut"
    };

public:
    explicit SceneManagerTestHelper(SceneManagerTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
        scenes = {
            {"../tests/textures/Scene_1.png", 3000, 1},
            {"../tests/textures/Scene_2.png", 3000, 2},
            {"../tests/textures/Scene_3.png", 3000, 3},
            {"../tests/textures/Scene_4.png", 3000, 4},
            {"../tests/textures/Scene_5.png", 3000, 5},
            {"../tests/textures/Scene_6.png", 3000, 6},
            {"../tests/textures/Scene_7.png", 3000, 7}
        };

        loadCurrentScene();
        sceneStartTime = std::chrono::steady_clock::now();
    }

    void loadCurrentScene() {
        const auto &scene = scenes[currentSceneIndex];
        lastTextureId = currentTextureId;

        if (texture tex; ctx.textureManager->load(scene.textureName, tex)) {
            currentTextureId = tex.textureProperties.id;
        } else {
            currentTextureId = -1;
            SDL_Log("Fehler beim Laden der Textur: %s", scene.textureName.c_str());
        }

        // Lade die nächste Textur für den Übergang
        int nextSceneIndex = (currentSceneIndex + 1) % scenes.size();
        GLuint nextTextureId = -1;
        if (texture nextTex; ctx.textureManager->load(scenes[nextSceneIndex].textureName, nextTex)) {
            nextTextureId = nextTex.textureProperties.id;
        }

        switch (scene.transitionType) {
            case 1: currentTransition = ctx.effectFactory->createFadeInEffect();
                break;
            case 2: currentTransition = ctx.effectFactory->createFadeOutEffect();
                break;
            case 3: currentTransition = ctx.effectFactory->createCrossFadeEffect();
                break;
            case 4: currentTransition = ctx.effectFactory->createSlideInEffect();
                break;
            case 5: currentTransition = ctx.effectFactory->createSlideOutEffect();
                break;
            case 6: currentTransition = ctx.effectFactory->createWipeInEffect();
                break;
            case 7: currentTransition = ctx.effectFactory->createWipeOutEffect();
                break;
            default: currentTransition = nullptr;
                break;
        }

        if (currentTransition) {
            currentTransition->activate();
            currentTransition->setTextures(currentTextureId, nextTextureId);
        }
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);

        switch (data.key) {
            case SDLK_SPACE:
                if (!isPlaying) {
                    const auto now = std::chrono::steady_clock::now();
                    const auto pauseDuration = now - pauseTime;
                    sceneStartTime += pauseDuration;
                } else {
                    pauseTime = std::chrono::steady_clock::now();
                }
                isPlaying = !isPlaying;
                break;
            case SDLK_RIGHT:
                currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
                loadCurrentScene();
                sceneStartTime = std::chrono::steady_clock::now();
                break;
            case SDLK_LEFT:
                currentSceneIndex = (currentSceneIndex - 1 + scenes.size()) % scenes.size();
                loadCurrentScene();
                sceneStartTime = std::chrono::steady_clock::now();
                break;
            case SDLK_r:
                currentSceneIndex = 0;
                loadCurrentScene();
                sceneStartTime = std::chrono::steady_clock::now();
                break;
            default:
                break;
        }
    }

    void update(const float deltaTime) {
        if (isPlaying) {
            const auto currentTime = std::chrono::steady_clock::now();
            const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - sceneStartTime).count();

            if (elapsedMilliseconds >= scenes[currentSceneIndex].duration) {
                currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
                loadCurrentScene();
                sceneStartTime = std::chrono::steady_clock::now();
            }
        }

        if (currentTransition) {
            currentTransition->update(deltaTime);
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentTextureId >= 0) {
            renderFullscreenTexture(currentTextureId);
        }

        if (currentTransition) {
            currentTransition->draw();
        }

        renderInstructions(deltaTime, instructions);

        const auto &[textureName, duration, transitionType] = scenes[currentSceneIndex];
        const auto currentTime = std::chrono::steady_clock::now();
        const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - sceneStartTime).count();

        float remainingTime = (duration - elapsedMilliseconds) / 1000.0f;
        remainingTime = std::max(0.0f, remainingTime);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        float yPos = -0.85f;
        const std::string sceneInfo = "Scene " + std::to_string(currentSceneIndex + 1) + "/" +
                                      std::to_string(scenes.size()) + ": " +
                                      textureName +
                                      " (Remaining: " + std::to_string(remainingTime).substr(0, 4) + "s)";
        ctx.textManager.write(sceneInfo, Fonts::IntroDescription, true, 1.0f, 0.0f, yPos);
        yPos -= 0.05f;

        const std::string transInfo = "Transition: " + transitionNames[transitionType];
        ctx.textManager.write(transInfo, Fonts::IntroDescription, true, 1.0f, 0.0f, yPos);
        yPos -= 0.05f;

        const std::string playStatus = isPlaying ? "Status: Playing" : "Status: Paused";
        ctx.textManager.write(playStatus, Fonts::IntroDescription, true, 1.0f, 0.0f, yPos);

        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

private:
    static void renderFullscreenTexture(const GLuint textureId) {
        if (textureId <= 0) return;

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }
};

int main() {
    try {
        SceneManagerTestContext ctx;
        EventDispatcher eventDispatcher(&ctx.eventManager);
        SceneManagerTestHelper testHelper(ctx);

        const std::vector<std::string> instructions = {
            "ESC: Quit",
            "SPACE: Pause/Play",
            "<- ->: Prev/Next Scene",
            "R: Restart",
            "M: Toggle Mouse Coordinates",
            "S: Screenshot"
        };

        bool running = true;
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();

            testHelper.update(deltaTime);
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
