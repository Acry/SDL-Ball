// SpriteSheetAnimationManager.cpp
#include "SpriteSheetAnimationManager.h"

bool SpriteSheetAnimationManager::registerForAnimation(GameObject *gameObject,
                                                       const SpriteSheetAnimationProperties &props) {
    if (!gameObject) return false;
    animations.emplace_back(gameObject, props);
    return true;
}

bool SpriteSheetAnimationManager::unregisterFromAnimation(GameObject *gameObject) {
    const auto it = std::ranges::find_if(animations,
                                         [gameObject](const AnimationBinding &binding) {
                                             return binding.gameObject == gameObject;
                                         });

    if (it != animations.end()) {
        animations.erase(it);
        return true;
    }

    return false;
}

void SpriteSheetAnimationManager::updateAllAnimations(const float deltaTime) {
    for (auto &binding: animations) {
        binding.animation.play(deltaTime);
        copyUVCoordinates(binding);
    }
}

bool SpriteSheetAnimationManager::setAnimationPlaying(const GameObject *gameObject, const bool playing) {
    for (auto &binding: animations) {
        if (binding.gameObject == gameObject) {
            binding.animation.isPlaying = playing;
            return true;
        }
    }
    return false;
}

bool SpriteSheetAnimationManager::setAnimationFrame(const GameObject *gameObject, const Uint32 frame) {
    for (auto &binding: animations) {
        if (binding.gameObject == gameObject) {
            binding.animation.currentFrame = frame;
            binding.animation.calculateTextureCoordinates();
            copyUVCoordinates(binding);
            return true;
        }
    }
    return false;
}

void SpriteSheetAnimationManager::resetAllAnimations() {
    for (auto &binding: animations) {
        binding.animation.reset();
        copyUVCoordinates(binding);
    }
}

void SpriteSheetAnimationManager::copyUVCoordinates(const AnimationBinding &binding) {
    for (int i = 0; i < 8; ++i) {
        binding.gameObject->uvCoordinates[i] = binding.animation.currentTexturePosition[i];
    }
}
