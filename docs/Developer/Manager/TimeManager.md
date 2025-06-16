# TimeManager

```c++
class TimeManager {
    public:
    static int getDeltaTime() { return isPaused ? 0 : lastFrameDelta; }
    static void updateDeltaTime() {
        Uint32 now = SDL_GetTicks();
        lastFrameDelta = now - lastFrameTime;
        lastFrameTime = now;
    }
    
    static void setPaused(bool paused) { isPaused = paused; }
    
    private:
    static Uint32 lastFrameTime;
    static int lastFrameDelta;
    static bool isPaused;
};

    // Dann in SpriteSheetAnimation.cpp:
    void SpriteSheetAnimation::play() {
    if (textureProperties.playing) {
    age += TimeManager::getDeltaTime();
        // ...
    }
}
```

see timing: ../Physics%20&%20Timing/Timing.md