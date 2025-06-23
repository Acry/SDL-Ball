# Screen Transitions

Those are effects and go to? the effectManager!

Use framebuffers to render scenes to textures, then draw these textures as full-screen quads with transformations (for
sliding) or shaders (for complex effects like cube or bar stripe).

Interpolate a transition progress value (0.0 to 1.0) over a fixed duration (e.g., 1 second) to animate the effect.

Horizontal Slide
The outgoing scene slides left out of view, while the incoming scene slides in from the right, creating a smooth
horizontal shift.

Complexity: Low. Requires basic matrix transformations.

Fade (Cross-Dissolve)
Description: The outgoing scene fades out (opacity decreases) while the incoming scene fades in (opacity increases),
blending the two images.

Complexity: Low. Requires basic shader blending.

Wipe (Left-to-Right)
Description: A vertical boundary moves left to right, revealing the incoming scene while hiding the outgoing scene.

Complexity: Low. Similar to fade but uses a conditional instead of blending.

Cube Transition
Description: The outgoing scene is mapped to one face of a cube, and the incoming scene to an adjacent face. The cube
rotates 90 degrees to reveal the new scene.

Complexity: Medium. Requires 3D geometry and perspective projection.

Bar Stripe Transition

Description: The screen is divided into horizontal or vertical bars. Each bar transitions from the outgoing to the
incoming scene with a slight delay, creating a striped effect.

Complexity: Medium. Requires a custom fragment shader but no geometry changes.

Copy Machine Transition
Description: A bright “scan line” moves across the screen (e.g., left to right), revealing the incoming scene behind it.
The outgoing scene may fade slightly as the line passes.

Complexity: Medium. Similar to wipe but adds a glowing line effect.

Zoom Transition
Description: The outgoing scene zooms out (scales down) while the incoming scene zooms in (scales up), optionally with
fading.

Complexity: Low. Similar to slide but uses scaling instead of translation.

Radial Wipe
Description: The incoming scene is revealed in a circular sweep, like a clock hand or radar, starting from the center.

Complexity: Medium. Requires trigonometric calculations in the shader.

Pixelation Transition
Description: The outgoing scene becomes increasingly pixelated, then the incoming scene emerges from the pixelated
state, becoming clear.

Complexity: Medium. Requires shader-based pixelation.

Page Curl
Description: The outgoing scene curls up like a page being turned, revealing the incoming scene underneath.

Complexity: High. Involves advanced shader math.

enum class TransitionType {
SLIDE_HORIZONTAL,
FADE,
WIPE,
CUBE,
BAR_STRIPE,
COPY_MACHINE,
ZOOM,
RADIAL_WIPE,
PIXELATION,
PAGE_CURL
};

https://gl-transitions.com/
https://tympanus.net/codrops/

