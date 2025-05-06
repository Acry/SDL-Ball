#include <iostream>
#include <epoxy/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include "display.hpp"

using namespace std;

bool displayClass::init(bool fullscreen, int displayToUseIn)
{
  int SDL_videomodeSettings = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

  //Save current resolution so it can be restored at exit
  // Declare display mode structure to be filled in.
  SDL_DisplayMode currentDisplayMode;

  SDL_Init(SDL_INIT_VIDEO);

  // Get current display mode of all displays.
  numOfDisplays = SDL_GetNumVideoDisplays();
  SDL_Rect displayBounds[numOfDisplays]={0};

  for(int i = 0; i < numOfDisplays; ++i){

    int should_be_zero = SDL_GetCurrentDisplayMode(i, &currentDisplayMode);

    if(should_be_zero != 0)
    {
      // In case of error...
      SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
    }
    else
    {
      // On success, print the current display mode.
      SDL_GetDisplayBounds( i, &displayBounds[i] );
      SDL_Rect currentDisplayBounds = displayBounds[i];
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. %d %d %d %d",
    		  i, currentDisplayMode.w, currentDisplayMode.h, currentDisplayMode.refresh_rate,
			  currentDisplayBounds.x,currentDisplayBounds.y,currentDisplayBounds.h,currentDisplayBounds.w);
    }
  }

  if((numOfDisplays > 0) && (displayToUseIn<numOfDisplays))
  {
	  displayToUse = displayToUseIn;
  }
  else
  {
	  displayToUse = 0;
  }

  currentW = displayBounds[displayToUse].w;
  currentH = displayBounds[displayToUse].h;

  //Initialize SDL
  #ifndef NOSOUND
  if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) <0 )
  #else
  if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_JOYSTICK) <0 )
  #endif
  {
    printf("\nError: Unable to initialize SDL:%s\n", SDL_GetError());
    return false;
  }

//  if(fullscreen)
//  {
//    SDL_videomodeSettings |= SDL_WINDOW_FULLSCREEN;
//  }
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  sdlWindow = SDL_CreateWindow("SDL-Ball",
		                    SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
							SDL_WINDOWPOS_CENTERED_DISPLAY(displayToUse),
							displayBounds[displayToUse].w, displayBounds[displayToUse].h,
							SDL_videomodeSettings);

  glcontext = SDL_GL_CreateContext(sdlWindow);

  resize(currentW, currentH);

  if( (sdlWindow == NULL) ||  (glcontext == NULL))
  {
    SDL_Log("Error:%s", SDL_GetError());
	return false;
  }
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	return true;
}

bool displayClass::updateForMenu()
{
	SDL_Log("displayClass::updateForMenu() called");
	return 1;
}

void displayClass::resize(int width, int height )
{
	SDL_Log("displayClass::resize() called");

	/* Height / width ration */
	GLfloat ratio;

	/* Protect against a divide by zero */
	if ( height == 0 )
		height = 1;

	currentW = width;
	currentH = height;

	ratio = ( GLfloat )width / ( GLfloat )height;
	glunits_per_xpixel = (2.485281374*ratio) / currentW;
	glunits_per_ypixel = 2.485281374 / currentH;


	/* Setup our viewport. */
	glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

	/* change to the projection matrix and set our viewing volume. */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	/* Set our perspective */
	gluPerspective( 45.0f, ratio, 0.1f, 10.0f );

	/* Make sure we're chaning the model view and not the projection */
	glMatrixMode( GL_MODELVIEW );

	/* Reset The View */
	glLoadIdentity();
}

void displayClass::close()
{
	SDL_DestroyWindow(sdlWindow);
	SDL_GL_DeleteContext(glcontext);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_Quit();
}
