#include "includes.h"

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
} 

///////////////////////////////////////////////////////////
//// NOTE(Elias): SDL Basics

internal S32 
SDL_initialise(SDL_Context *sdl_context)
{
  B32 err_code = 0;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LogErrString("failed to initliase SDL!");
    err_code = 1;
    goto err1;
  }
  sdl_context->window = SDL_CreateWindow("Lupy Niceguy Game", 
                                         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!sdl_context->window) {
    LogErrString("failed to initliase SDL Window!");
    SDL_Quit();
    err_code = 2;
    goto err2;
  }
  sdl_context->surface = SDL_GetWindowSurface(sdl_context->window);
  SDL_FillRect(sdl_context->surface, 0, 
               SDL_MapRGB(sdl_context->surface->format, 0x00 , 0x00 , 0x00 ));
  SDL_UpdateWindowSurface(sdl_context->window);
err2:
err1:
  return(err_code);
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
  S32 err_code = 0;
  SDL_Context sdl_context = {}; 
 
  S32 start_tick;
  U64 counter; 
  GameInput game_input = {}; 
  GameState *game_state;

  if (SDL_initialise(&sdl_context) != 0) {
    LogErrString("failed to initliase SDL");
    err_code = 1;
    goto err1;
  }

  game_state = (GameState *)calloc(1, sizeof(GameState));
  if (!game_state) {
    LogErrString("failed to allocate memory for the game state!");
    err_code = 2;
    goto err2; 
  }

  game_initialise(game_state, sdl_context.surface);
    
  // NOTE(Elias): Game loop
  while (global_running)
  { 
    start_tick = SDL_GetTicks(); 
    
    SDL_process_pending_messages(&game_input); 
    SDL_process_keyboard(&game_input); 
   
    // NOTE(Elias): Could be that we need to intertwine these
    // two into a function game_update_and_render(...);
    game_update(game_state, &game_input, sdl_context.surface, counter); 
    game_render(sdl_context.surface, game_state); 
    
    SDL_UpdateWindowSurface(sdl_context.window); 
    
    ++counter; 
    
    // NOTE(Elias): cap framerate
    if ((1000.0 / FPS) > (SDL_GetTicks() - start_tick)) {
      SDL_Delay((1000.0 / FPS) - (F64)(SDL_GetTicks() - start_tick));
    }
  }

  game_die(game_state);
  free(game_state);

err2:
  SDL_die(&sdl_context);
err1:
  return(err_code);
}


