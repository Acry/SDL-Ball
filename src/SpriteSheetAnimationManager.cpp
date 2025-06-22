// SpriteSheetAnimationManager.cpp
#include "SpriteSheetAnimationManager.h"

#include <SDL2/SDL_log.h>

bool SpriteSheetAnimationManager::registerForAnimation(GameObject *gameObject,
                                                       SpriteSheetAnimationProperties &props,
                                                       TextureCoordinates &uvCoords) {
    if (!gameObject) return false;

    // Prüfen ob das GameObject bereits registriert ist
    for (const auto &binding: animations) {
        if (binding->gameObject == gameObject && binding->propertiesPtr == &props) {
            return false; // Object ist bereits mit diesen Properties registriert
        }
    }

    auto binding = std::make_unique<AnimationBinding>(gameObject, props, uvCoords);
    animations.push_back(std::move(binding));
    return true;
}

bool SpriteSheetAnimationManager::unregisterFromAnimation(GameObject *gameObject,
                                                          SpriteSheetAnimationProperties &props) {
    const auto it = std::ranges::find_if(animations,
                                         [gameObject, &props](const auto &binding) {
                                             return binding->gameObject == gameObject &&
                                                    binding->propertiesPtr == &props;
                                         });

    if (it != animations.end()) {
        // if (gameObject) {
        //     // FixMe - Reset UV coordinates to first frame
        //     // Reset UV coordinates to default values
        //     ((*it)->targetCoordinates)[0] = 0.0f;
        //     ((*it)->targetCoordinates)[1] = 0.0f; // Bottom-left
        //     ((*it)->targetCoordinates)[2] = 1.0f;
        //     ((*it)->targetCoordinates)[3] = 0.0f; // Bottom-right
        //     ((*it)->targetCoordinates)[4] = 1.0f;
        //     ((*it)->targetCoordinates)[5] = 1.0f; // Top-right
        //     ((*it)->targetCoordinates)[6] = 0.0f;
        //     ((*it)->targetCoordinates)[7] = 1.0f; // Top-left
        // }
        animations.erase(it);
        return true;
    }

    return false;
}

void SpriteSheetAnimationManager::updateAllAnimations(const float deltaTime) const {
    for (auto &binding: animations) {
        binding->animation.play(deltaTime);
        copyUVCoordinates(*binding);
    }
}

bool SpriteSheetAnimationManager::setAnimationPlaying(const GameObject *gameObject,
                                                      const SpriteSheetAnimationProperties &props,
                                                      const bool playing) const {
    for (const auto &binding: animations) {
        if (binding->gameObject == gameObject && binding->propertiesPtr == &props) {
            binding->animation.isPlaying = playing;
            return true;
        }
    }
    return false;
}

bool SpriteSheetAnimationManager::setAnimationFrame(const GameObject *gameObject,
                                                    const SpriteSheetAnimationProperties &props,
                                                    const Uint32 frame) const {
    for (auto &binding: animations) {
        if (binding->gameObject == gameObject && binding->propertiesPtr == &props) {
            binding->animation.currentFrame = frame;
            binding->animation.calculateTextureCoordinates();
            copyUVCoordinates(*binding);
            return true;
        }
    }
    return false;
}

void SpriteSheetAnimationManager::resetAllAnimations() const {
    for (auto &binding: animations) {
        binding->animation.reset();
        copyUVCoordinates(*binding);
    }
}

void SpriteSheetAnimationManager::copyUVCoordinates(const AnimationBinding &binding) {
    binding.targetCoordinates = binding.animation.currentTexturePosition;
}
