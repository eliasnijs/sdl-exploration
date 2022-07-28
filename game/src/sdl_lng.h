struct SDL_Context 
{
  SDL_Window  *window;
  SDL_Surface *surface;
};

internal B32 
SDL_initialise(SDL_Context *sdl_context);

internal void 
SDL_die(SDL_Context *sdl_context);

internal void
SDL_process_pending_messages(GameInput *game_input);

internal void
SDL_process_keyboard_input(GameButtonState *state, B32 is_down);
