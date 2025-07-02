// Ball_Tests.cpp
#include <cstdlib>
#include <numeric>
#include <vector>
#include <cmath>
#include <algorithm>
#include "DisplayManager.hpp"
#include "EventDispatcher.h"
#include "GrowableObject.h"
#include "MathHelper.h"
#include "MouseManager.h"
#include "TestHelper.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "SpriteSheetAnimationManager.h"


class TestBall final : public GrowableObject, public ICollideable {
public:
    explicit TestBall(const texture &tex) : GrowableObject(tex) {
        this->width = 0.03f;
        this->height = 0.03f;
        this->velocity = 0.5f;
        onSizeChanged();
    }

    float centerX{0.0f};
    float centerY{0.0f};
    mutable std::vector<float> collisionPoints;

    void init() override {
        // Implementierung der abstrakten Methode aus GameObject
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

    // Ball-spezifische Methoden
    [[nodiscard]] float getAngle() const {
        return angleRadians;
    }

    void setAngle(const float o) {
        angleRadians = o;
        xvel = velocity * std::cos(angleRadians);
        yvel = velocity * std::sin(angleRadians);
    }

    void setSpeed(const float v) {
        velocity = v;
        xvel = velocity * std::cos(angleRadians);
        yvel = velocity * std::sin(angleRadians);
    }

    void setSize(const float s) {
        setGrowTarget(s);
    }

    std::vector<float> *getCollisionPoints() const {
        // Statisches Array für die Kreispunkte zur Kollisionserkennung
        static std::vector<float> points;
        points.clear();
        points.reserve(64); // 32 Punkte * 2 Koordinaten

        // Mittelpunkt des Balls berechnen
        const float centerX = pos_x + width / 2.0f;
        const float centerY = pos_y + height / 2.0f;

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

    // Getter und Setter für TestBall-spezifische Eigenschaften
    [[nodiscard]] bool isPhysicallyActive() const { return active; }
    void setPhysicallyActive(const bool value) { active = value; }
    [[nodiscard]] bool isGlued() const { return glued; }
    void setGlued(const bool value) { glued = value; }
    [[nodiscard]] bool isExplosive() const { return explosive; }
    void setExplosive(const bool value) { explosive = value; }

private:
    bool collisionActive{true};
    bool active{false};
    bool glued{false};
    bool explosive{false};
    float angleRadians{0.0f};
};

class TestBallManager {
    IEventManager *eventManager;
    TextureManager *textureManager;
    SpriteSheetAnimationManager *animationManager;
    std::vector<size_t> animationIndices; // Indizes der Bälle mit Animation
    static constexpr int MAX_BALLS = 50;
    float launchAngle = M_PI / 2 + (rand() % 40 - 20) * 0.01f; // π/2 ± kleine Zufallsabweichung
public:
    TestBall *selectedBall{nullptr};
    std::vector<TestBall *> managedObjects;

    TestBallManager(IEventManager *evtMgr, TextureManager *texMgr, SpriteSheetAnimationManager *animMgr)
        : eventManager(evtMgr), textureManager(texMgr), animationManager(animMgr) {
    }

    void setExplosive(TestBall *ball, bool explosive) {
        if (!ball) return;

        // Finde den Ball-Index
        const auto it = std::ranges::find(managedObjects, ball);
        if (it == managedObjects.end()) return;

        const size_t ballIndex = std::distance(managedObjects.begin(), it);
        ball->setExplosive(explosive);

        if (explosive) {
            const texture *fireballTexture = textureManager->getBallTexture(BallTexture::Fireball);
            if (fireballTexture) {
                ball->textureProperties = fireballTexture->textureProperties;
                ball->animProps = fireballTexture->animationProperties;
                // Animation registrieren, falls vorhanden
                if (ball->animProps.frames > 1) {
                    animationManager->
                            registerForAnimation(ball, ball->animProps, ball->textureProperties.uvCoordinates);
                    animationIndices.push_back(ballIndex);
                }
            }
        } else {
            const texture *normalTexture = textureManager->getBallTexture(BallTexture::Normal);
            if (normalTexture) {
                ball->textureProperties = normalTexture->textureProperties;
                ball->animProps = normalTexture->animationProperties;
                animationManager->unregisterFromAnimation(ball, ball->animProps);
                std::erase(animationIndices, ballIndex);
            }
        }
    }

    void despawnBall(TestBall *ball) {
        if (!ball) return;

        ball->setPhysicallyActive(false);
        animationManager->unregisterFromAnimation(ball, ball->animProps);

        if (const auto it = std::ranges::find(managedObjects, ball); it != managedObjects.end()) {
            const size_t ballIndex = std::distance(managedObjects.begin(), it);
            std::erase(animationIndices, ballIndex);
            managedObjects.erase(it);
        }
        delete ball;
    }

    void spawn(const float x, const float y, const bool glued = false, const float speed = 0.3f,
               const float angle = 0.0f) {
        for (auto *ball: managedObjects) {
            if (!ball->isPhysicallyActive()) {
                ball->setPhysicallyActive(true);
                ball->setGlued(glued);
                // Setze centerX/centerY und berechne daraus pos_x/pos_y
                ball->centerX = x;
                ball->centerY = y;
                ball->pos_x = x - ball->width / 2.0f;
                ball->pos_y = y - ball->height / 2.0f;
                selectedBall = ball;
                launchBall();
                return;
            }
        }
        if (managedObjects.size() < MAX_BALLS) {
            const texture *ballTexture = textureManager->getBallTexture(BallTexture::Normal);
            if (!ballTexture) {
                SDL_Log("Fehler: Konnte Ball-Textur nicht laden");
                return;
            }

            const auto ball = new TestBall(*ballTexture);
            ball->setPhysicallyActive(true);
            ball->setGlued(glued);
            ball->centerX = x;
            ball->centerY = y;
            ball->pos_x = x - ball->width / 2.0f;
            ball->pos_y = y - ball->height / 2.0f;
            if (ball->animProps.frames > 1) {
                animationManager->registerForAnimation(ball, ball->animProps, ball->textureProperties.uvCoordinates);
            }
            managedObjects.push_back(ball);
            selectedBall = ball;
            launchBall();
        }
    }

    void update(const float deltaTime) const {
        for (auto *ball: managedObjects) {
            if (!ball->isPhysicallyActive() || ball->isGlued()) continue;
            ball->update(deltaTime);
        }
        checkBorderCollision();
        checkBallToBallCollision();
    }

    static float getRandomLaunchAngle() {
        // Generates an angle of π/2 ± 0.087 radians (approx. ±5 degrees)
        return M_PI / 2 + randomFloat(0.174f, 0.087f);
    }

    void launchBall() const {
        selectedBall->setGlued(false);
        const float randomAngle = getRandomLaunchAngle();
        selectedBall->xvel = selectedBall->velocity * std::cos(randomAngle);
        selectedBall->yvel = selectedBall->velocity * std::sin(randomAngle);
        selectedBall->setAngle(randomAngle);
    }

    void checkBorderCollision() const {
        for (auto *ball: managedObjects) {
            if (!ball->isPhysicallyActive()) continue;

            if (ball->pos_x + ball->width > 1.0f) {
                ball->pos_x = 1.0f - ball->width;
                ball->xvel *= -1;
            } else if (ball->pos_x < -1.0f) {
                ball->pos_x = -1.0f;
                ball->xvel *= -1;
            }

            if (ball->pos_y + ball->height > 1.0f) {
                ball->pos_y = 1.0f - ball->height;
                ball->yvel *= -1;
            } else if (ball->pos_y < -1.0f) {
                ball->pos_y = -1.0f;
                ball->yvel *= -1;
            }
        }
    }

    void checkBallToBallCollision() const {
        for (size_t i = 0; i < managedObjects.size(); ++i) {
            auto *ball1 = managedObjects[i];
            if (!ball1->isPhysicallyActive()) continue;

            for (size_t j = i + 1; j < managedObjects.size(); ++j) {
                auto *ball2 = managedObjects[j];
                if (!ball2->isPhysicallyActive()) continue;

                // Einfache Kreis-zu-Kreis Kollisionserkennung
                const float dx = ball2->centerX - ball1->centerX;
                const float dy = ball2->centerY - ball1->centerY;
                const float distance = std::sqrt(dx * dx + dy * dy);

                const float radius1 = ball1->width / 2.0f;
                const float radius2 = ball2->width / 2.0f;

                if (distance < radius1 + radius2) {
                    // Kollision erkannt

                    // Verhindern von Division durch Null
                    if (distance < 0.0001f) continue;

                    // Normalisierte Richtungsvektoren
                    const float nx = dx / distance;
                    const float ny = dy / distance;

                    // Überlappung berechnen
                    const float overlap = (radius1 + radius2) - distance;

                    // Bälle auseinanderschieben (Überlappung auflösen)
                    ball1->pos_x -= overlap * nx * 0.5f;
                    ball1->pos_y -= overlap * ny * 0.5f;
                    ball2->pos_x += overlap * nx * 0.5f;
                    ball2->pos_y += overlap * ny * 0.5f;

                    // Zentren aktualisieren
                    ball1->centerX = ball1->pos_x + ball1->width / 2.0f;
                    ball1->centerY = ball1->pos_y + ball1->height / 2.0f;
                    ball2->centerX = ball2->pos_x + ball2->width / 2.0f;
                    ball2->centerY = ball2->pos_y + ball2->height / 2.0f;

                    // Massen der Bälle berechnen (proportional zum Volumen/Radius³)
                    const float mass1 = std::powf(radius1, 3);
                    const float mass2 = std::powf(radius2, 3);

                    // Relativgeschwindigkeit
                    const float rvx = ball2->xvel - ball1->xvel;
                    const float rvy = ball2->yvel - ball1->yvel;

                    // Geschwindigkeit entlang der Kollisionsnormale
                    const float velAlongNormal = rvx * nx + rvy * ny;

                    // Nur reagieren, wenn sich die Bälle aufeinander zubewegen
                    if (velAlongNormal > 0) continue;

                    // Elastischer Stoß mit ungleichen Massen
                    constexpr float restitution = 1.0f; // Vollkommen elastisch

                    // Impulsberechnung nach Impulserhaltungssatz
                    const float impulse = -(1.0f + restitution) * velAlongNormal / (1.0f / mass1 + 1.0f / mass2);

                    // Geschwindigkeitsänderungen berechnen
                    const float dvx1 = -impulse * nx / mass1;
                    const float dvy1 = -impulse * ny / mass1;
                    const float dvx2 = impulse * nx / mass2;
                    const float dvy2 = impulse * ny / mass2;

                    // Neue Geschwindigkeiten anwenden
                    ball1->xvel += dvx1;
                    ball1->yvel += dvy1;
                    ball2->xvel += dvx2;
                    ball2->yvel += dvy2;

                    // Mindestgeschwindigkeit für beide Bälle sicherstellen
                    constexpr float MIN_SPEED = 0.3f;

                    // Ball 1 prüfen und korrigieren
                    float speed1 = std::sqrt(ball1->xvel * ball1->xvel + ball1->yvel * ball1->yvel);
                    if (speed1 < MIN_SPEED && speed1 > 0.0001f) {
                        // Behalte die Richtung bei, aber erhöhe die Geschwindigkeit
                        ball1->xvel = ball1->xvel * MIN_SPEED / speed1;
                        ball1->yvel = ball1->yvel * MIN_SPEED / speed1;
                    }

                    // Ball 2 prüfen und korrigieren
                    float speed2 = std::sqrt(ball2->xvel * ball2->xvel + ball2->yvel * ball2->yvel);
                    if (speed2 < MIN_SPEED && speed2 > 0.0001f) {
                        // Behalte die Richtung bei, aber erhöhe die Geschwindigkeit
                        ball2->xvel = ball2->xvel * MIN_SPEED / speed2;
                        ball2->yvel = ball2->yvel * MIN_SPEED / speed2;
                    }

                    // Kollisionspunkte aktualisieren
                    ball1->collisionPoints = *ball1->getCollisionPoints();
                    ball2->collisionPoints = *ball2->getCollisionPoints();
                }
            }
        }
    }

    void draw() const {
        for (const auto *ball: managedObjects) {
            if (ball->isActive()) ball->draw();
            if (ball == selectedBall) {
                glLoadIdentity();
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Rot
                glLineWidth(0.1f);
                glBegin(GL_LINE_LOOP);
                glVertex3f(ball->pos_x, ball->pos_y, 0.0f);
                glVertex3f(ball->pos_x + ball->getWidth(), ball->pos_y, 0.0f);
                glVertex3f(ball->pos_x + ball->getWidth(), ball->pos_y + ball->getHeight(), 0.0f);
                glVertex3f(ball->pos_x, ball->pos_y + ball->getHeight(), 0.0f);
                glEnd();
                glLineWidth(1.0f);
                glDisable(GL_BLEND);
            }
        }
    }

    void clear() {
        for (auto *ball: managedObjects) {
            animationManager->unregisterFromAnimation(ball, ball->animProps);
            ball->setPhysicallyActive(false);
        }
        animationIndices.clear();
    }

    ~TestBallManager() {
        clear(); // Code-Duplikation vermeiden

        // Anschließend Speicher freigeben
        for (auto *ball: managedObjects) {
            delete ball;
        }
        managedObjects.clear();
    }
};

class BallTestContext {
public:
    EventManager eventManager;
    MouseManager mouseManager;
    DisplayManager displayManager;
    TextManager textManager;
    std::unique_ptr<TextureManager> textureManager;
    SpriteSheetAnimationManager animationManager;
    std::unique_ptr<TestBallManager> ballManager;

    BallTestContext()
        : mouseManager(&eventManager),
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
        ballManager = std::make_unique<TestBallManager>(&eventManager, textureManager.get(), &animationManager);
    }
};

class BallTestHelper final : public TestHelper {
    BallTestContext &ctx;

public:
    explicit BallTestHelper(BallTestContext &context)
        : TestHelper(context.textManager, &context.eventManager), ctx(context) {
    }

    void handleKeyPress(const KeyboardEventData &data) override {
        TestHelper::handleKeyPress(data);
        switch (data.key) {
            case SDLK_e:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const bool currentState = ctx.ballManager->selectedBall->isExplosive();
                    ctx.ballManager->setExplosive(ctx.ballManager->selectedBall, !currentState);
                }
                break;
            case SDLK_g:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const float currentSize = ctx.ballManager->selectedBall->getWidth();
                    float newSize = currentSize * 1.2f;
                    if (newSize <= 0.2f) {
                        ctx.ballManager->selectedBall->grow(ctx.ballManager->selectedBall->getWidth() * 1.2f);
                    }
                }
                break;
            case SDLK_k:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    const float currentSize = ctx.ballManager->selectedBall->width;
                    float newSize = currentSize * 0.8f;
                    if (newSize >= 0.01f) {
                        ctx.ballManager->selectedBall->grow(ctx.ballManager->selectedBall->getWidth() * 0.8f);
                    }
                }
                break;
            case SDLK_DELETE:
                if (ctx.ballManager && ctx.ballManager->selectedBall) {
                    TestBall *currentBall = ctx.ballManager->selectedBall;
                    ctx.ballManager->despawnBall(currentBall);
                    ctx.ballManager->selectedBall = nullptr;

                    // Suche nach dem nächsten aktiven Ball
                    for (auto *ball: ctx.ballManager->managedObjects) {
                        if (ball->isPhysicallyActive()) {
                            ctx.ballManager->selectedBall = ball;
                            break;
                        }
                    }
                }
            default:
                break;
        }
    }

    void handleMouseButton(const MouseEventData &data) override {
        TestHelper::handleMouseButton(data);
        if (data.button == SDL_BUTTON_LEFT) {
            if (ctx.ballManager) {
                ctx.ballManager->spawn(m_mouseX, m_mouseY, false);
            }
        }
    }

    void MouseWheelScrolled(const MouseEventData &data) override {
        TestHelper::MouseWheelScrolled(data);
        if (ctx.ballManager->managedObjects.empty()) {
            return;
        }
        std::vector<TestBall *> activeBalls;
        for (auto *ball: ctx.ballManager->managedObjects) {
            if (ball->isPhysicallyActive()) {
                activeBalls.push_back(ball);
            }
        }

        if (activeBalls.empty()) {
            return;
        }
        const auto currentBallIt = std::find(activeBalls.begin(), activeBalls.end(), ctx.ballManager->selectedBall);
        size_t currentIndex = 0;

        if (currentBallIt != activeBalls.end()) {
            currentIndex = std::distance(activeBalls.begin(), currentBallIt);
        }

        // Berechne den nächsten Ball-Index basierend auf der Rad-Bewegung
        int nextIndex;
        if (data.wheelY < 0) {
            // Nach unten scrollen (vorwärts)
            nextIndex = (currentIndex + 1) % activeBalls.size();
        } else if (data.wheelY > 0) {
            // Nach oben scrollen (rückwärts)
            nextIndex = (currentIndex + activeBalls.size() - 1) % activeBalls.size();
        } else {
            // Keine vertikale Bewegung
            return;
        }

        ctx.ballManager->selectedBall = activeBalls[nextIndex];
    }

    void render(const float deltaTime, const std::vector<std::string> &instructions) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawGrid();
        drawCenterLines();
        ctx.animationManager.updateAllAnimations(deltaTime);
        ctx.ballManager->update(deltaTime);
        ctx.ballManager->draw();
        renderInstructions(deltaTime, instructions);
        drawBallCount();
        drawMouseCoordinates();
        SDL_GL_SwapWindow(ctx.displayManager.sdlWindow);
    }

    void drawBallCount() const {
        int activeBalls = 0;
        for (const auto *ball: ctx.ballManager->managedObjects) {
            if (ball->isPhysicallyActive()) {
                activeBalls++;
            }
        }
        char ballCountText[32];
        sprintf(ballCountText, "Balls: %d", activeBalls);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        ctx.textManager.write(ballCountText, Fonts::Highscore, true, 0.4f, 0.0f, -0.9f);
    }
};

int main() {
    try {
        BallTestContext ctx;
        BallTestHelper testHelper(ctx);
        EventDispatcher eventDispatcher(&ctx.eventManager);
        const std::vector<std::string> instructions = {
            "LMB: Spawn Ball",
            "Vertical Mouse Wheel: Select Ball",
            "E: Toggle Explosive State of Selected Ball",
            "G: Grow Selected Ball",
            "K: Shrink Selected Ball",
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
