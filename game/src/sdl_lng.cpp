#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>

#include "baselayer.h"
#include "config.h"

#include "lng.h"
#include "lng.cpp"
#include "sdl_lng.h"

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
SDL_process_keyboard_input(GameButtonState *state, B32 is_down)
{
  state->ended_down = is_down;
}

internal void
SDL_process_pending_messages(GameInput *game_input)
{
  SDL_Event sdl_event; 
  while (SDL_PollEvent(&sdl_event))
  {
    B32 key_is_down = false;
    switch (sdl_event.type) {
      case (SDL_QUIT): 
      {
        global_running = false;
      } break;
      case (SDL_MOUSEMOTION): 
      {
        game_input->mouse_x = sdl_event.motion.x; game_input->mouse_y = sdl_event.motion.y;
      } break;
      case (SDL_MOUSEBUTTONDOWN):
        key_is_down = true;
      case (SDL_MOUSEBUTTONUP):
      {
        U8 button = sdl_event.button.button;
        if (button == SDL_BUTTON_LEFT) 
        {
          SDL_process_keyboard_input(&game_input->mouse_left, key_is_down);
        }
        else if (button == SDL_BUTTON_RIGHT)
        {
          SDL_process_keyboard_input(&game_input->mouse_right, key_is_down);
        }
      } break;
      case (SDL_KEYDOWN): 
        key_is_down = true;
        // NOTE(Elias): no break;
      case (SDL_KEYUP): 
      {
        // TODO(Elias): handle modded keystates (shift, ctrl, ...)
        // TODO(Elias): still flawed, take a closer look 
        U32 key = sdl_event.key.keysym.sym;
        if (key == SDLK_w) 
        { 
          SDL_process_keyboard_input(&game_input->move_up, key_is_down);
        }
        else if (key == SDLK_a) 
        { 
          SDL_process_keyboard_input(&game_input->move_left, key_is_down);
        }
        else if (key == SDLK_s) 
        { 
          SDL_process_keyboard_input(&game_input->move_down, key_is_down);
        }
        else if (key == SDLK_d) 
        { 
          SDL_process_keyboard_input(&game_input->move_right, key_is_down);
        }
        else if (key == SDLK_UP) 
        { 
          SDL_process_keyboard_input(&game_input->move_up, key_is_down);
        }
        else if (key == SDLK_DOWN) 
        { 
          SDL_process_keyboard_input(&game_input->move_down, key_is_down);
        }
        else if (key == SDLK_LEFT) 
        { 
          SDL_process_keyboard_input(&game_input->move_left, key_is_down);
        }
        else if (key == SDLK_RIGHT) 
        { 
          SDL_process_keyboard_input(&game_input->move_right, key_is_down);
        }
        else if (key == SDLK_SPACE)
        { 
          SDL_process_keyboard_input(&game_input->action1, key_is_down);
        }
        else if (key == SDLK_RETURN)
        { 
        }
        else if (key == SDLK_ESCAPE)
        { 
          SDL_process_keyboard_input(&game_input->action8, key_is_down);
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
    // NOTE(Elias): Initialisation
    S32 start_tick; 
    S64 counter = 0;
    GameState game_state = {};
    GameInput game_input = {}; 
    
    game_initialise(&game_state, sdl_context.surface);
    
    // NOTE(Elias): Game loop
    while (global_running)
    { 
      
      start_tick = SDL_GetTicks(); 
      
      SDL_process_pending_messages(&game_input); 
      
      if (game_input.action8.ended_down)
      {
        continue;  
      }

      game_update_and_render(&game_state, &game_input, sdl_context.surface, counter); 
      SDL_UpdateWindowSurface(sdl_context.window); 
      ++counter;
      
      // NOTE(Elias): cap framerate
      if ((1000.0 / FPS) > (SDL_GetTicks() - start_tick)) 
      {
        SDL_Delay((1000.0 / FPS) - (F64)(SDL_GetTicks() - start_tick));
      }
    }

  }
  else 
  {
    printf("failed to initliase SDL\n");
    success = false;
  }
  SDL_die(&sdl_context);
  
  return(!success);
}
