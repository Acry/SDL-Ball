// SpriteSheetAnimationManager.h
#pragma once

#include <vector>

#include "GameObject.h"
#include "SpriteSheetAnimation.h"

class SpriteSheetAnimationManager {
public:
    /**
     * Registriert ein GameObject für Animationen.
     * @param gameObject Das zu registrierende GameObject.
     * @param props Die Animationseigenschaften für das GameObject.
     * @param uvCoords
     * @return True wenn erfolgreich registriert, false sonst.
     */
    bool registerForAnimation(GameObject *gameObject, SpriteSheetAnimationProperties &props,
                              TextureCoordinates &uvCoords);

    /**
     * Entfernt ein GameObject aus dem Manager.
     * @param gameObject Das zu entfernende GameObject.
     * @return True wenn erfolgreich entfernt, false wenn nicht gefunden.
     */
    bool unregisterFromAnimation(GameObject *gameObject, SpriteSheetAnimationProperties &props);

    /**
     * Aktualisiert alle registrierten Animationen mit der angegebenen Deltazeit.
     * @param deltaTime Die seit dem letzten Update vergangene Zeit in Sekunden.
     */
    void updateAllAnimations(float deltaTime) const;

    /**
     * Setzt den Abspielstatus einer Animation für ein bestimmtes GameObject.
     * @param gameObject Das GameObject, dessen Animation gesteuert werden soll.
     * @param props Die Animationseigenschaften für die spezifische Animation.
     * @param playing True zum Abspielen, False zum Pausieren.
     * @return True wenn erfolgreich gesetzt, false wenn GameObject nicht gefunden.
     */
    bool setAnimationPlaying(const GameObject *gameObject, const SpriteSheetAnimationProperties &props,
                             bool playing) const;

    /**
     * Setzt den aktuellen Frame für die Animation eines GameObjects.
     * @param gameObject Das GameObject.
     * @param props Die Animationseigenschaften für die spezifische Animation.
     * @param frame Der zu setzende Frame.
     * @return True wenn erfolgreich gesetzt, false wenn GameObject nicht gefunden.
     */
    bool setAnimationFrame(const GameObject *gameObject, const SpriteSheetAnimationProperties &props,
                           Uint32 frame) const;

    /**
     * Setzt alle Animationen zurück.
     */
    void resetAllAnimations() const;

private:
    struct AnimationBinding {
        GameObject *gameObject;
        SpriteSheetAnimation animation;
        SpriteSheetAnimationProperties *propertiesPtr;
        TextureCoordinates &targetCoordinates;

        AnimationBinding(GameObject *obj,
                         SpriteSheetAnimationProperties &props,
                         TextureCoordinates &target)
            : gameObject(obj),
              animation(props),
              propertiesPtr(&props),
              targetCoordinates(target) {
        }
    };

    std::vector<std::unique_ptr<AnimationBinding> > animations;

    static void copyUVCoordinates(const AnimationBinding &binding);
};
