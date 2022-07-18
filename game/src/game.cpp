#include <SDL2/SDL.h>

#include <stdio.h>

#include "baselayer.h"
#include "config.h"

struct SDL_Context 
{
  SDL_Window  *window;
  SDL_Surface *screen_surface;
};

global_variable B32 global_running = true;

internal B32 
SDL_initialise(SDL_Context *sdl_context)
{
  B32 success = true;
  if (SDL_Init(SDL_INIT_VIDEO) >= 0)
  {
    sdl_context->window = SDL_CreateWindow("Lupy Niceguy Game", 
                                           SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                           SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (sdl_context->window)
    {
      sdl_context->screen_surface = SDL_GetWindowSurface(sdl_context->window);
      SDL_FillRect(sdl_context->screen_surface, 0, 
                   SDL_MapRGB(sdl_context->screen_surface->format, 0x00, 0x00, 0x00));
      SDL_UpdateWindowSurface(sdl_context->window);
    }
    else
    {
      // TODO (Elias): logging
      success = false;
    }
  }
  else
  {
    // TODO (Elias): logging
    success = false;
  }
  return(success);
}

internal void 
SDL_die(SDL_Context *sdl_context)
{
  SDL_DestroyWindow(sdl_context->window);
  SDL_Quit();
}

S32 
main()
{
  B32 success = true;
  SDL_Context sdl_context = {};

  if (SDL_initialise(&sdl_context))
  {
    SDL_Event sdl_event; 
    while (global_running)
    {
      // NOTE(Elias): Poll events
      while (SDL_PollEvent(&sdl_event))
      {
        switch (sdl_event.type) {
          case (SDL_QUIT): {
            global_running = false;
          } break;
        }
      }
      // NOTE(Elias): update windoww surface
      SDL_UpdateWindowSurface(sdl_context.window);
    }
  }
  else 
  {
    // TODO(Elias): Logging 
  }
  SDL_die(&sdl_context);
  return(!success);
}


