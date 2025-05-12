# Jimmy Christensens SDL-Ball

I pulled the Repository via <https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=sdl-ball>.
With sound enabled, the rendering was broken for me.
I had to initialize the soundClass before creating the window and opengl-context.

Check out the following commit for the fixed code, while the rest stays the same:

2de3f8ca2dc5c2de78da38bcbb6a55dd238df17a

`git checkout 2de3f8c`

## Work in Progress

I am currently working on a refactoring of the code to make it more modern and maintainable.
Current Target is OpenGL 4.6 and SDL3.

See: [get started](docs/get_started.md)
or
See: [Todo](docs/Todo.md)

## Significant changes

I am going to modernize the code in my spare time for educational purposes.

- Projection change to 2D-Orthographic
- OpenGL 3.1 compatibility profile
- no Windows support (macOS should be easy to add)
- no WiiMote support
- sound always enabled

The config is now in `.config/SDL-Ball`, not `sdl-ball` anymore.
You can copy the files from the old folder to the new one, should just work.
