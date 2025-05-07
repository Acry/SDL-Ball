# Jimmy Christensens SDL-Ball

I pulled the Repository via <https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=sdl-ball>.
With sound enabled, the rendering was broken for me.
I had to initialize the soundClass before creating the window and opengl-context.

Check out the following commit for the fixed code, while the rest stays the same:

2de3f8ca2dc5c2de78da38bcbb6a55dd238df17a

`git checkout 2de3f8c`

___

I am going to modernize the code in my spare time for educational purposes.
No cross-platform support is intended. Current commits will use the OpenGL 3.1 compatibility profile already.

The config is now in `SDL-Ball`, not `sdl-ball` anymore.
You can copy the files from the old folder to the new one.
