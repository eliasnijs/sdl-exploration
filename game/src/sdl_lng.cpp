#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdint.h>

#include "baselayer.h"
#include "baselayer.cpp"
#include "config.h"

#include "lng.h"
#include "lng.cpp"
#include "sdl_lng.h"


///////////////////////////////////////////////////////////
//// NOTE(Elias): Global Variables

global_variable B32 global_running = true;

///////////////////////////////////////////////////////////
//// NOTE(Elias): SDL Keyboard 

internal void
SDL_process_keyboard_input(GameButtonState *state, B32 is_down)
{
  state->was_down = state->ended_down;
  state->ended_down = is_down;
}

internal void
SDL_process_keyboard(GameInput *game_input)
{
  const U8 *state = SDL_GetKeyboardState(0); 
  SDL_process_keyboard_input(&game_input->move_up, 
                             state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]); 
  SDL_process_keyboard_input(&game_input->move_left, 
                             state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]); 
  SDL_process_keyboard_input(&game_input->move_down, 
                             state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]); 
  SDL_process_keyboard_input(&game_input->move_right, 
                             state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]); 
  SDL_process_keyboard_input(&game_input->action1, 
                             state[SDL_SCANCODE_SPACE]); 
  SDL_process_keyboard_input(&game_input->action8, 
                             state[SDL_SCANCODE_ESCAPE]); 
} 

///////////////////////////////////////////////////////////
//// NOTE(Elias): SDL Basics

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
                   SDL_MapRGB(sdl_context->surface->format, 0x00 , 0x00 , 0x00 ));
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
  SDL_FreeSurface(sdl_context->surface);
  SDL_DestroyWindow(sdl_context->window);
  SDL_Quit();
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
        game_input->mouse_x = sdl_event.motion.x; 
        game_input->mouse_y = sdl_event.motion.y;
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
    }
  }
}

///////////////////////////////////////////////////////////
//// NOTE(Elias): MAIN

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
    // NOTE(Elias): Is there another way to allocate memory instead of malloc?
    GameState *game_state = (GameState *)malloc(sizeof(GameState));
    GameInput game_input = {}; 
    
    game_initialise(game_state, sdl_context.surface);
    
    // NOTE(Elias): Game loop
    while (global_running)
    { 
      
      start_tick = SDL_GetTicks(); 
      
      SDL_process_pending_messages(&game_input); 
      SDL_process_keyboard(&game_input);

      // NOTE(Elias): Temporary pauze button
      if (game_input.action8.ended_down)
      {
        continue;  
      }

      game_update(game_state, &game_input, sdl_context.surface, counter); 
      game_render(sdl_context.surface, game_state);
      
      SDL_UpdateWindowSurface(sdl_context.window); 
      ++counter;
      
      // NOTE(Elias): cap framerate
      if ((1000.0 / FPS) > (SDL_GetTicks() - start_tick)) 
      {
        SDL_Delay((1000.0 / FPS) - (F64)(SDL_GetTicks() - start_tick));
      }
    }

    game_die(game_state);
    free(game_state);

  }
  else 
  {
    printf("failed to initliase SDL\n");
    success = false;
  }
  SDL_die(&sdl_context);
  
  return(!success);
}


