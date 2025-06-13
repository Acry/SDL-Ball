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
     * @return True wenn erfolgreich registriert, false sonst.
     */
    bool registerForAnimation(GameObject *gameObject, const SpriteSheetAnimationProperties &props);

    /**
     * Entfernt ein GameObject aus dem Manager.
     * @param gameObject Das zu entfernende GameObject.
     * @return True wenn erfolgreich entfernt, false wenn nicht gefunden.
     */
    bool unregisterFromAnimation(GameObject *gameObject);

    /**
     * Aktualisiert alle registrierten Animationen mit der angegebenen Deltazeit.
     * @param deltaTime Die seit dem letzten Update vergangene Zeit in Sekunden.
     */
    void updateAllAnimations(float deltaTime);

    /**
     * Setzt den Abspielstatus einer Animation für ein bestimmtes GameObject.
     * @param gameObject Das GameObject, dessen Animation gesteuert werden soll.
     * @param playing True zum Abspielen, False zum Pausieren.
     * @return True wenn erfolgreich gesetzt, false wenn GameObject nicht gefunden.
     */
    bool setAnimationPlaying(const GameObject *gameObject, bool playing);

    /**
     * Setzt den aktuellen Frame für die Animation eines GameObjects.
     * @param gameObject Das GameObject.
     * @param frame Der zu setzende Frame.
     * @return True wenn erfolgreich gesetzt, false wenn GameObject nicht gefunden.
     */
    bool setAnimationFrame(const GameObject *gameObject, Uint32 frame);

    /**
     * Setzt alle Animationen zurück.
     */
    void resetAllAnimations();

private:
    struct AnimationBinding {
        GameObject *gameObject;
        SpriteSheetAnimation animation;

        AnimationBinding(GameObject *obj, const SpriteSheetAnimationProperties &props)
            : gameObject(obj), animation(props) {
        }
    };

    std::vector<AnimationBinding> animations;

    static void copyUVCoordinates(const AnimationBinding &binding);
};
