# Menu

```c++
if (var.menu) {
    if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.78) + (0.07) && mouse_y > (-0.78) - (0.07))
        var.menuItem = 1;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.56) + (0.07) && mouse_y > (-0.56) - (
                 0.07))
        var.menuItem = 2;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.34) + (0.07) && mouse_y > (-0.34) - (
                 0.07))
        var.menuItem = 3;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (-0.12) + (0.07) && mouse_y > (-0.12) - (
                 0.07))
        var.menuItem = 4;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.1) + (0.07) && mouse_y > (0.1) - (0.07))
        var.menuItem = 5;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.32) + (0.07) && mouse_y > (0.32) - (0.07))
        var.menuItem = 6;
    else if (mouse_x > -0.5 && mouse_x < 0.5 && mouse_y < (0.54) + (0.07) && mouse_y > (0.54) - (0.07))
        var.menuItem = 7;
    else
        var.menuItem = 0;
```

```c++
// In der Event-Handling-Schleife
SDL_Event event;
while(SDL_PollEvent(&event)) {
    if(event.type == SDL_MOUSEMOTION) {
        // Bildschirmkoordinaten in OpenGL-Koordinaten umrechnen
        float mouseX = (event.motion.x / (float)SCREEN_WIDTH) * 2.0f - 1.0f;
        float mouseY = -((event.motion.y / (float)SCREEN_HEIGHT) * 2.0f - 1.0f);

        // Menü-Position startet bei y=0.54
        float itemY = 0.54f;
        float itemWidth = 0.425f;  // half_width aus dem Menu-Code
        float itemHeight = 0.07f;
        
        // Prüfe Kollision mit jedem Menüeintrag
        for(int i = 0; i < var.menuNumItems; i++) {
            if(mouseX >= -itemWidth && mouseX <= itemWidth &&
               mouseY >= itemY - itemHeight && mouseY <= itemY + itemHeight) {
                var.menuItem = var.menuNumItems - i;
                break;
            }
            itemY -= 0.22f; // Abstand zwischen Menüeinträgen
        }
    }
}
```