// SpriteSheetAnimationManager_Tests.cpp
#include <cstdlib>
#include <numeric>

#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "MouseManager.h"
#include "SpriteSheetAnimationManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"

std::vector<GameObject *> testObjects;

class TestGameObject final : public GameObject {
public:
    explicit TestGameObject(const texture &tex) : GameObject(tex) {
        pos_x = -1.0f;
        pos_y = 0.5f;
        width = 0.1f;
        height = 0.1f;
    }

    void init() override {
    }

    void draw() const override {
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, textureProperties.id);
        glColor4fv(textureProperties.textureColor);
        glBegin(GL_QUADS);

        // Bottom-left corner
        glTexCoord2f(textureProperties.uvCoordinates[0], textureProperties.uvCoordinates[1]);
        glVertex3f(pos_x, pos_y, 0.0f);

        // Bottom-right corner
        glTexCoord2f(textureProperties.uvCoordinates[2], textureProperties.uvCoordinates[3]);
        glVertex3f(pos_x + width, pos_y, 0.0f);

        // Top-right corner
        glTexCoord2f(textureProperties.uvCoordinates[4], textureProperties.uvCoordinates[5]);
        glVertex3f(pos_x + width, pos_y + height, 0.0f);

        // Top-left corner
        glTexCoord2f(textureProperties.uvCoordinates[6], textureProperties.uvCoordinates[7]);
        glVertex3f(pos_x, pos_y + height, 0.0f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
};

class ObjectManager {
public:
    explicit ObjectManager(SpriteSheetAnimationManager *animMgr) : animationManager(animMgr) {
    }

    template<typename T = GameObject>
    GameObject *createGameObject(texture &tex) {
        static_assert(std::is_base_of_v<GameObject, T>, "T muss von GameObject abgeleitet sein");
        auto *obj = new T(tex);
        managedObjects.push_back(obj);

        // Beim AnimationManager registrieren, wenn das Objekt Animationseigenschaften hat
        if (tex.animationProperties.frames > 1) {
            animationManager->registerForAnimation(obj, tex.animationProperties, obj->textureProperties.uvCoordinates);
        }
        return obj;
    }

    void destroyGameObject(GameObject *obj) {
        if (!obj) return;

        // Vom AnimationManager abmelden
        animationManager->unregisterFromAnimation(obj, obj->animProps);

        // Aus der Liste entfernen und löschen
        if (const auto it = std::ranges::find(managedObjects, obj); it != managedObjects.end()) {
            managedObjects.erase(it);
            delete obj;
        }
    }

private:
    SpriteSheetAnimationManager *animationManager;
    std::vector<GameObject *> managedObjects;
};

int main() {
    EventManager eventManager;
    MouseManager mouseManager(&eventManager);
    DisplayManager displayManager(&eventManager);
    if (!displayManager.init(0, 1024, 768, false)) {
        SDL_Log("Could not initialize display");
        return EXIT_FAILURE;
    }
    SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: SpriteSheetManager Test");

    TextManager textManager(&eventManager);
    if (!textManager.setTheme("../tests/themes/test")) {
        SDL_Log("Fehler beim Laden des Font-Themes");
        return EXIT_FAILURE;
    }
    EventDispatcher eventDispatcher(&eventManager);
    TestHelper testHelper(textManager, &eventManager);

    texture texture{};
    const std::filesystem::path texturePath = "../themes/default/gfx/powerup/gun";
    TextureManager textureManager;
    if (!textureManager.loadTextureWithProperties(texturePath, texture)) {
        SDL_Log("Fehler beim Laden der Textur: %s", texturePath.c_str());
        return EXIT_FAILURE;
    }

    SpriteSheetAnimationManager animationManager;
    ObjectManager objectManager(&animationManager);

    constexpr int rows = 9;
    for (int row = 0; row < rows; row++) {
        constexpr int cols = 10;
        for (int col = 0; col < cols; col++) {
            constexpr float startY = 0.7f;
            constexpr float ySpacing = 0.2f;
            GameObject *testObj = objectManager.createGameObject<TestGameObject>(texture);
            testObj->pos_x = -0.95f + col * 0.2f;
            testObj->pos_y = startY - row * ySpacing;
            testObjects.push_back(testObj);
        }
    }

    //GameObject *testObj = objectManager.createGameObject<TestGameObject>(texture);
    //testObj->pos_x = -0.95f;
    //testObj->pos_y = 0.2f;
    //testObj->width = 0.4f;
    //testObj->height = 0.4f;

    //testObjects.push_back(testObj);
    const std::vector<std::string> instructions = {
        "S: Create Screenshot",
        "M: Toggle Mouse Coordinates",
        "ESC: Quit"
    };

    bool running = true;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    std::vector<float> renderTimes;
    renderTimes.reserve(100);

    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        running = eventDispatcher.processEvents();
        animationManager.updateAllAnimations(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        testHelper.drawGrid();
        testHelper.drawCenterLines();

        auto renderStartTime = std::chrono::high_resolution_clock::now();
        for (auto *obj: testObjects) {
            obj->draw();
        }
        auto renderEndTime = std::chrono::high_resolution_clock::now();
        float renderDuration = std::chrono::duration<float, std::milli>(renderEndTime - renderStartTime).count();
        renderTimes.push_back(renderDuration);

        testHelper.renderInstructions(deltaTime, instructions);
        testHelper.drawMouseCoordinates();
        SDL_GL_SwapWindow(displayManager.sdlWindow);
    }
    for (auto *obj: testObjects) {
        objectManager.destroyGameObject(obj);
    }
    if (!renderTimes.empty()) {
        float avgRenderTime = std::accumulate(renderTimes.begin(), renderTimes.end(), 0.0f) / renderTimes.size();
        float minRenderTime = *std::min_element(renderTimes.begin(), renderTimes.end());
        float maxRenderTime = *std::max_element(renderTimes.begin(), renderTimes.end());

        SDL_Log("Average rendertime: %.2f ms (Min: %.2f ms, Max: %.2f ms)",
                avgRenderTime, minRenderTime, maxRenderTime);
    }
    return EXIT_SUCCESS;
}
