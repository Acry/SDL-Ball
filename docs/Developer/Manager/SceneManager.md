# SceneManager

SceneManager is responsible for managing the different scenes in the game.
Der ThemeManager muss aufgebohrt werden. snd (Audio, Graphics, Level, Fonts, Powerups, Backgrounds, Title (Scenes?)).

- [ ] add a new scene for the used keys
- [ ] add a new scene for more explanation, see Website page=about.html
- [ ] autoplay after title for a while
- [ ] separate Powerup Descriptions from Title, new Scene
- [ ] credits scene
- [ ] TitleScreen, rework
  Blocker GameManager, SceneManager, MenuManager
- [ ] TitleScreen_Tests
- [ ] SceneManager create
- [ ] SceneManager_Tests
- [ ] add a pause indicator / Pause Scene
- [ ] show current level no# during gameplay, IDK get ready for level X, fade in paddle and ball, play sounds

## See

[Scenes](../Scenes/Intro.md) for a list of all scenes.

## sketch

#ifndef SCENE_H
#define SCENE_H

class Scene {
public:
virtual ~Scene() = default;
virtual void initialize() = 0; // Called when scene is first loaded
virtual void update(float deltaTime) = 0; // Update logic
virtual void render() = 0; // Render the scene
virtual void cleanup() = 0; // Called when scene is unloaded
};

    void addScene(const std::string& name, std::unique_ptr<Scene> scene) {
        scenes[name] = std::move(scene);
    }

    // Set the active scene by name
    void setActiveScene(const std::string& name) {
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            if (activeScene) {
                activeScene->cleanup(); // Clean up previous scene
            }
            activeScene = it->second.get();
            activeScene->initialize(); // Initialize new scene
        }
    }

    // Update the active scene
    void update(float deltaTime) {
        if (activeScene) {
            activeScene->update(deltaTime);
        }
    }

    // Remove a scene by name
    void removeScene(const std::string& name) {
        scenes.erase(name);
    }

    // Clear all scenes
    void clearScenes() {
        scenes.clear();
        activeScene = nullptr;
    }

    // Get the current active scene's name
    std::string getActiveSceneName() const {
        for (const auto& pair : scenes) {
            if (pair.second.get() == activeScene) {
                return pair.first;
            }
        }
        return "";
    }

private:
std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
Scene* activeScene; // Pointer to the currently active scene
};

Add a TransitionType currentTransition member to SceneManager.
In setActiveScene, set the desired transition type (e.g., randomly or based on settings from SettingsManager).
In render, switch on currentTransition to select the appropriate shader and rendering logic:

void render() {
activeScene->render(incomingFbo->fbo);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float t = transitionTime / transitionDuration;
    switch (currentTransition) {
        case TransitionType::SLIDE_HORIZONTAL:
            renderSlide(t);
            break;
        case TransitionType::FADE:
            renderFade(t);
            break;
        // Add cases for other transitions
    }

}


