struct GameButtonState 
{
  B32 ended_down; 
};

union GameInput 
{
  GameButtonState game_buttons[12];
  struct 
  {
    GameButtonState move_up; 
    GameButtonState move_left; 
    GameButtonState move_right; 
    GameButtonState move_down; 
    GameButtonState action1;
    GameButtonState action2;
    GameButtonState action3;
    GameButtonState action4;
    GameButtonState action5;
    GameButtonState action6;
    GameButtonState action7;
    GameButtonState action8;
  };
};

struct GameState
{
  B32 is_initialised;
  S32 offset1;
  S32 offset2;
  S32 offset3;

  S32 player_x;
  S32 player_y;

};

internal void
render_pattern(SDL_Surface *surface, S32 offset1, S32 offset2, S32 offset3);

void
game_update_and_render(GameState *game_state, GameInput *game_input, SDL_Surface *surface);
