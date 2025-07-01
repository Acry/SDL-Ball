// Paddle_Tests.cpp
#include <cstdlib>
#include <numeric>
#include <vector>
#include <cmath>
#include <algorithm>
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "GrowableObject.h"
#include "KeyboardManager.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"

class TestPaddle final : public GrowableObject, public ICollideable {
public:
    explicit TestPaddle(const texture &tex) : GrowableObject(tex) {
        this->width = 0.124f;
        this->height = 0.032f;
        this->velocity = 0.0f;
        pos_y = -0.955f;
        pos_x = 0.0f - width / 2.0f;
        aspectRatio = width / height;
        onSizeChanged();
    }

    float centerX{0.0f};
    float centerY{0.0f};

    mutable std::vector<float> collisionPoints;

    TextureResource glueLayerTextureProperties;
    SpriteSheetAnimationProperties glueLayerAnimProps{};

    TextureResource gunLayerTextureProperties;
    SpriteSheetAnimationProperties gunLayerAnimProps{};

    void init() override {
        // Implementierung der abstrakten Methode aus GameObject
    }

    void draw() const override {
        drawBase();
        if (hasGlueLayer) {
            drawGlueLayer();
        }
        if (hasGunLayer) {
            drawGunLayer();
        }
    }

    void update(const float deltaTime) override {
        GrowableObject::update(deltaTime);
        centerX = pos_x + width / 2.0f;
        centerY = pos_y + height / 2.0f;
        collisionPoints = *getCollisionPoints();
    }

    // ICollideable Interface
    void setActive(const bool value) override { collisionActive = value; }
    [[nodiscard]] float getPosX() const override { return pos_x; }
    [[nodiscard]] float getPosY() const override { return pos_y; }
    [[nodiscard]] float getWidth() const override { return width; }
    [[nodiscard]] float getHeight() const override { return height; }
    [[nodiscard]] bool isActive() const override { return collisionActive; }

    [[nodiscard]] CollisionType getCollisionType() const override {
        return CollisionType::Ball;
    }

    // Paddle-spezifische Methoden
    void drawBase() const {
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

    void drawGlueLayer() const {
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, glueLayerTextureProperties.id);
        glColor4fv(glueLayerTextureProperties.textureColor);
        glBegin(GL_QUADS);

        // Bottom-left corner
        glTexCoord2f(glueLayerTextureProperties.uvCoordinates[0], glueLayerTextureProperties.uvCoordinates[1]);
        glVertex3f(pos_x, pos_y, 0.0f);

        // Bottom-right corner
        glTexCoord2f(glueLayerTextureProperties.uvCoordinates[2], glueLayerTextureProperties.uvCoordinates[3]);
        glVertex3f(pos_x + width, pos_y, 0.0f);

        // Top-right corner
        glTexCoord2f(glueLayerTextureProperties.uvCoordinates[4], glueLayerTextureProperties.uvCoordinates[5]);
        glVertex3f(pos_x + width, pos_y + height, 0.0f);

        // Top-left corner
        glTexCoord2f(glueLayerTextureProperties.uvCoordinates[6], glueLayerTextureProperties.uvCoordinates[7]);
        glVertex3f(pos_x, pos_y + height, 0.0f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    void drawGunLayer() const {
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, gunLayerTextureProperties.id);
        glColor4fv(gunLayerTextureProperties.textureColor);
        glBegin(GL_QUADS);


        // Bottom-left corner
        glTexCoord2f(gunLayerTextureProperties.uvCoordinates[0], gunLayerTextureProperties.uvCoordinates[1]);
        glVertex3f(pos_x, pos_y + height, 0.0f);

        // Bottom-right corner
        glTexCoord2f(gunLayerTextureProperties.uvCoordinates[2], gunLayerTextureProperties.uvCoordinates[3]);
        glVertex3f(pos_x + width, pos_y + height, 0.0f);

        // the 3.0f is hardcoded on top of the paddle
        // actually one needs the normalized texture height and divide it by rows

        // Top-right corner
        glTexCoord2f(gunLayerTextureProperties.uvCoordinates[4], gunLayerTextureProperties.uvCoordinates[5]);
        glVertex3f(pos_x + width, pos_y + height + height / 3.0f, 0.0f);

        // Top-left corner
        glTexCoord2f(gunLayerTextureProperties.uvCoordinates[6], gunLayerTextureProperties.uvCoordinates[7]);
        glVertex3f(pos_x, pos_y + height + height / 3.0f, 0.0f);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    void setGlueLayer(const bool enabled) {
        hasGlueLayer = enabled;
    }

    bool getGlueLayer() const {
        return hasGlueLayer;
    }

    void setGunLayer(const bool enabled) {
        hasGunLayer = enabled;
    }

    bool getGunLayer() const {
        return hasGunLayer;
    }

    std::vector<float> *getCollisionPoints() const {
        // Statisches Array für die Kreispunkte zur Kollisionserkennung
        static std::vector<float> points;
        points.clear();
        points.reserve(64); // 32 Punkte * 2 Koordinaten

        // Mittelpunkt des Balls berechnen
        const float centerX = pos_x + width / 2.0f;
        const float centerY = pos_y + height / 2.0f;

        // FIXME: Paddle segments
        // Kreispunkte um den Ball erzeugen
        for (int i = 0; i < 32; ++i) {
            const float angle = i * (2.0f * M_PI / 32.0f);
            const float pointX = centerX + std::cos(angle) * (width / 2.0f);
            const float pointY = centerY + std::sin(angle) * (height / 2.0f);
            points.push_back(pointX);
            points.push_back(pointY);
        }

        return &points;
    }

    void onSizeChanged() override {
        //collisionPoints = *getCollisionPoints();
    }

    [[nodiscard]] bool isPhysicallyActive() const { return active; }
    void setPhysicallyActive(const bool value) { active = value; }

private:
    bool collisionActive{true};
    bool active{false};
    bool hasGlueLayer{false};
    bool hasGunLayer{false};
};

class TestPaddleManager {
    IEventManager *eventManager;
    TextureManager *textureManager;
    SpriteSheetAnimationManager *animationManager;
    std::vector<size_t> animationIndices;
    float moveTargetX = 0.0f;

public:
    TestPaddle *activePaddle = nullptr;
    float paddleSpeed = 10.0f; // Anpassbare Geschwindigkeit
    [[nodiscard]] TestPaddle *getActivePaddle() const {
        return activePaddle;
    }

    void setGlueLayer(const bool enabled) const {
        if (activePaddle) {
            activePaddle->setGlueLayer(enabled);
            if (enabled && activePaddle->glueLayerAnimProps.frames > 1) {
                animationManager->registerForAnimation(activePaddle, activePaddle->glueLayerAnimProps,
                                                       activePaddle->glueLayerTextureProperties.uvCoordinates);
            } else if (!enabled && activePaddle->glueLayerAnimProps.frames > 1) {
                animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
            }
        }
    }

    void setGunLayer(const bool enabled) const {
        if (activePaddle) {
            activePaddle->setGunLayer(enabled);
            if (enabled && activePaddle->gunLayerAnimProps.frames > 1) {
                animationManager->registerForAnimation(activePaddle, activePaddle->gunLayerAnimProps,
                                                       activePaddle->gunLayerTextureProperties.uvCoordinates);
            } else if (!enabled && activePaddle->gunLayerAnimProps.frames > 1) {
                animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
            }
        }
    }

    TestPaddleManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr)
        : eventManager(evtMgr), textureManager(texMgr), animationManager(animMgr) {
        eventManager->addListener(GameEvent::MouseCoordinatesNormalized,
                                  [this](const MouseCoordinatesNormalizedEventData &data) {
                                      this->handleMouseCoordinatesNormalized(data);
                                  }, this);
        eventManager->addListener(GameEvent::KeyboardPaddleMove,
                                  [this](const KeyboardMoveEventData &data) {
                                      this->handleKeyboardMove(data);
                                  }, this);
    }

    void handleMouseCoordinatesNormalized(const MouseCoordinatesNormalizedEventData &data) {
        if (activePaddle) {
            paddleSpeed = 10.0f;
            moveTargetX = data.x - activePaddle->getWidth() / 2.0f;
        }
    }

    void handleKeyboardMove(const KeyboardMoveEventData &data) {
        if (activePaddle) {
            if (data.direction != 0.0f) {
                moveTargetX = data.direction < 0 ? -1.0f : 1.0f;
            } else {
                moveTargetX = activePaddle->pos_x;
            }
            paddleSpeed = 1.0f;
        }
    }

    void despawn() {
        if (activePaddle) {
            activePaddle->setPhysicallyActive(false);
            if (activePaddle->animProps.frames > 1) {
                animationManager->unregisterFromAnimation(activePaddle, activePaddle->animProps);
            }
            if (activePaddle->getGlueLayer() && activePaddle->glueLayerAnimProps.frames > 1) {
                animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
            }
            if (activePaddle->getGunLayer() && activePaddle->gunLayerAnimProps.frames > 1) {
                animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
            }
            const TestPaddle *oldPaddle = activePaddle;
            activePaddle = nullptr;
            delete oldPaddle;
        }
    }

    void spawn() {
        const texture *paddleBaseTexture = textureManager->getPaddleTexture(PaddleTexture::Base);
        const auto paddle = new TestPaddle(*paddleBaseTexture);

        if (paddle->animProps.frames > 1) {
            animationManager->registerForAnimation(paddle, paddle->animProps,
                                                   paddle->textureProperties.uvCoordinates);
        }

        const texture *paddleGlueTexture = textureManager->getPaddleTexture(PaddleTexture::Glue);
        paddle->glueLayerTextureProperties = paddleGlueTexture->textureProperties;
        paddle->glueLayerAnimProps = paddleGlueTexture->animationProperties;

        const texture *paddleGunTexture = textureManager->getPaddleTexture(PaddleTexture::Gun);
        paddle->gunLayerTextureProperties = paddleGunTexture->textureProperties;
        paddle->gunLayerAnimProps = paddleGunTexture->animationProperties;

        paddle->setPhysicallyActive(true);
        activePaddle = paddle;
    }

    void update(const float deltaTime) const {
        if (activePaddle) {
            activePaddle->update(deltaTime);
            moveTowardsTarget(deltaTime);
            checkBorderCollision();
        }
    }

    void moveTowardsTarget(const float deltaTime) const {
        if (moveTargetX == activePaddle->pos_x) {
            return;
        }

        // Richtung bestimmen
        float direction = (moveTargetX > activePaddle->pos_x) ? 1.0f : -1.0f;

        // Konstante Geschwindigkeitsbewegung
        float movement = direction * paddleSpeed * deltaTime;

        // Prüfen, ob wir das Ziel mit dieser Bewegung überschreiten würden
        if ((direction > 0 && activePaddle->pos_x + movement > moveTargetX) ||
            (direction < 0 && activePaddle->pos_x + movement < moveTargetX)) {
            // Genau auf Zielposition setzen, um Überschwingen zu vermeiden
            activePaddle->pos_x = moveTargetX;
        } else {
            // Mit konstanter Geschwindigkeit bewegen
            activePaddle->pos_x += movement;
        }
    }

    void checkBorderCollision() const {
        if (activePaddle) {
            if (activePaddle->pos_x + activePaddle->width > 1.0f) {
                activePaddle->pos_x = 1.0f - activePaddle->width;
            } else if (activePaddle->pos_x < -1.0f) {
                activePaddle->pos_x = -1.0f;
            }
        }
    }

    void checkPaddleToPowerupCollision() const {
        // FIXME: TO PowerupManager
        ;
    }

    void draw() const {
        if (activePaddle) {
            activePaddle->draw();
        }
    }

    void clear() {
        if (activePaddle) {
            EventData data;
            data.sender = activePaddle;
            eventManager->emit(GameEvent::PaddleDestroyed, data); // FIXME
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->animProps);
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->glueLayerAnimProps);
            animationManager->unregisterFromAnimation(activePaddle, activePaddle->gunLayerAnimProps);
            activePaddle->setPhysicallyActive(false);
            delete activePaddle;
            activePaddle = nullptr;
        }
    }

    ~TestPaddleManager() {
        eventManager->removeListener(GameEvent::MouseCoordinatesNormalized, this);
        eventManager->removeListener(GameEvent::KeyboardPaddleMove, this);
        clear();
    }
};

class PaddleTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    KeyboardManager keyboardManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    SpriteSheetAnimationManager animationManager;
    std::unique_ptr<TestPaddleManager> paddleManager;

    PaddleTestContext()
        : mouseManager(&eventManager),
          keyboardManager(&eventManager),
          displayManager(&eventManager),
          textManager(&eventManager) {
        if (!displayManager.init(0, 1024, 768, false)) {
            throw std::runtime_error("Could not initialize display");
        }
        SDL_SetWindowTitle(displayManager.sdlWindow, "SDL-Ball: Ball Test");
        textManager.setTheme("../tests/themes/test");
        textureManager = std::make_unique<TextureManager>();
        if (!textureManager->setSpriteTheme("../themes/default")) {
            throw std::runtime_error("Error loading texture theme");
        }
        paddleManager = std::make_unique<TestPaddleManager>(&eventManager, textureManager.get(),
                                                            &animationManager);
    }
};

class PaddleTestHelper final : public TestHelper {
    PaddleTestContext &ctx;

public:
    explicit PaddleTestHelper(PaddleTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_w:
                if (!ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->spawn();
                }
                break;
            case SDLK_DELETE:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->despawn();
                }
                break;
            case SDLK_g:
                if (ctx.paddleManager->activePaddle) {
                    const float currentSize = ctx.paddleManager->activePaddle->getWidth();
                    float newSize = currentSize * 1.2f;
                    if (newSize <= 0.35f) {
                        ctx.paddleManager->activePaddle->grow(
                            ctx.paddleManager->activePaddle->getWidth() * 1.2f);
                    }
                }
                break;
            case SDLK_k:
                if (ctx.paddleManager->activePaddle) {
                    const float currentSize = ctx.paddleManager->activePaddle->getWidth();
                    float newSize = currentSize * 0.8f;
                    if (newSize >= 0.1f) {
                        ctx.paddleManager->activePaddle->grow(
                            ctx.paddleManager->activePaddle->getWidth() * 0.8f);
                    }
                }
                break;
            case SDLK_u:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->setGlueLayer(!ctx.paddleManager->activePaddle->getGlueLayer());
                }
                break;
            case SDLK_l:
                if (ctx.paddleManager->activePaddle) {
                    ctx.paddleManager->setGunLayer(!ctx.paddleManager->activePaddle->getGunLayer());
                }
            default:
                break;
        }
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        ctx.animationManager.updateAllAnimations(deltaTime);
        ctx.paddleManager->update(deltaTime);
        ctx.paddleManager->draw();
        renderInstructions(deltaTime, instructions);
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }
};

int main() {
    try {
        PaddleTestContext ctx;
        PaddleTestHelper testHelper(ctx);
        const EventDispatcher eventDispatcher(&ctx.eventManager);
        const std::vector<std::string> instructions = {
            "W: Spawn Paddle",
            "DEL: DeSpawn Paddle",
            "G: Grow Paddle",
            "K: Shrink Paddle",
            "LMB: Fire Gun, when Paddle has Gun Layer",
            "U: Toggle Glue State of Paddle",
            "L: Toggle Laser State of Paddle",
            "S: Screenshot",
            "M: Toggle Mouse Coordinates",
            "ESC: Quit"
        };

        bool running = true;
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            running = eventDispatcher.processEvents();
            testHelper.render(deltaTime, instructions);
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        SDL_Log("Error: %s", e.what());
        return EXIT_FAILURE;
    }
}
