# Jimmy Christensens SDL-Ball

![old logo](images/old-logo.png)

I pulled the Repository via <https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=sdl-ball>.
With sound enabled, the rendering was broken for me.
I had to initialize the soundClass before creating the window and opengl-context.

Check out the following commit for the fixed code, while the rest stays the same:

2de3f8ca2dc5c2de78da38bcbb6a55dd238df17a

`git checkout 2de3f8c`

## Work in Progress

Many comments are in German, that's a todo.
I am going to modernize the code in my spare time for educational purposes.
I spent a week on refactoring so far, and I am not done yet. Not every checkout builds.
I am currently working on a refactoring of the code to make it more modern and maintainable.
Current Target is OpenGL 4.6 and SDL3.
Probably I have time on weekends to work on it. Feel free to commit pull requests.

See: [get started](docs/Developer/get_started.md)
or
See: [Todo](docs/Developer/Todo.md)

## Significant changes

- Projection change to 2D-Orthographic
- Using OpenGL 3.1 compatibility profile See: [Projection](docs/Developer/OpenGL/Projection.md)
- no Windows support (macOS should be easy to add)
- no WiiMote support
- sound always enabled

The config is now in `.config/SDL-Ball`, not `sdl-ball` anymore.
You can copy the files from the old folder to the new one, should just work.

![new logo](images/current-logo.png)