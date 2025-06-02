# Animation-Timing

With Caching enabled, the animation timing does accumulate the time of all objects.
Eine Lösung wäre, die Animation zentral zu steuern, statt sie pro Brick zu aktualisieren.

1.

void SpriteSheetAnimation::play(const float deltaTime) {
static float animationAge = 0.0f; // Gemeinsamer Timer für alle Instanzen

    if (textureProperties.playing) {
        animationAge += deltaTime * 1000.0f;
        if (animationAge >= textureProperties.ticks) {
            animationAge = 0.0f;
            // Rest der Animation wie gehabt...

2.

Die Animation im TextureManager verwalten und die Bricks nur die aktuelle Frame-Position abfragen lassen: