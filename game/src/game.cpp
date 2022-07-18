#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>

#include "baselayer.h"
#include "config.h"

struct SDL_Context 
{
  SDL_Window  *window;
  SDL_Surface *surface;
};

global_variable B32 global_running = true;

internal void
render_pattern(SDL_Surface *surface, S32 offset)
{
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  S32 pattern_length = 2000;
  for (S32 row = 0;
       row < surface->h;
       ++row)
  {
    for (S32 column = 0;
         column < surface->w;
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      U8 red_channel = (U8) 255;
      U8 green_channel = (U8) ((row + offset) % pattern_length);
      U8 blue_channel =  (U8) ((column + offset) % pattern_length);
      *(U32 *)p = (red_channel << 16) | (green_channel << 8) | (blue_channel);
    }
  }
}

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
      sdl_context->surface = SDL_GetWindowSurface(sdl_context->window);
      SDL_FillRect(sdl_context->surface, 0, 
                   SDL_MapRGB(sdl_context->surface->format, 0x00, 0x00, 0x00));
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

internal void
SDL_process_pending_messages()
{
  SDL_Event sdl_event; 
  while (SDL_PollEvent(&sdl_event))
  {
    switch (sdl_event.type) {
      case (SDL_QUIT): {
        global_running = false;
      } break;
      case (SDL_KEYDOWN):
      case (SDL_KEYUP): {
        // TODO(Elias): handle modded keystates (shift, ctrl, ...)
        // TODO(Elias): still flawed, take a closer look 
        U32 key = sdl_event.key.keysym.sym;
        if (key == SDLK_w) 
        { 
        }
        else if (key == SDLK_a) 
        { 
        }
        else if (key == SDLK_s) 
        { 
        }
        else if (key == SDLK_d) 
        { 
        }
        else if (key == SDLK_UP) 
        { 
        }
        else if (key == SDLK_DOWN) 
        { 
        }
        else if (key == SDLK_LEFT) 
        { 
        }
        else if (key == SDLK_RIGHT) 
        { 
        }
        else if (key == SDLK_SPACE)
        { 
        }
        else if (key == SDLK_RETURN)
        { 
        }
        else if (key == SDLK_ESCAPE)
        { 
        }
      } break;
    }
  }
}

S32 
main()
{
  B32 success = true;
  SDL_Context sdl_context = {};

  if (SDL_initialise(&sdl_context))
  {
    S32 counter = 0;
    S32 start_tick; 
    while (global_running)
    { 
      ++counter;
      start_tick = SDL_GetTicks();
      
      SDL_process_pending_messages();
      
      render_pattern(sdl_context.surface, counter);
      
      SDL_UpdateWindowSurface(sdl_context.window);
      
      if ((1000 / FPS) > (SDL_GetTicks() - start_tick)) 
      {
        SDL_Delay((1000 / FPS) - (SDL_GetTicks() - start_tick));
      }
    }
  }
  else 
  {
    // TODO(Elias): Logging 
  }
  SDL_die(&sdl_context);
  
  return(!success);
}
